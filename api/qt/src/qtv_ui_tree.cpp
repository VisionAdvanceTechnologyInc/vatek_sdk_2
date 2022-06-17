//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2022, Vision Advance Technology Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//

#include <ui/base/qtv_ui_tree.h>

qtvUITreeBase::qtvUITreeBase(qui_tree_mode mode,QWidget* parent) :
	m_mode(mode),
	QTreeWidget(parent)
{
	setHeaderHidden(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

qtvUITreeBase::~qtvUITreeBase()
{

}

void qtvUITreeBase::resetTree(qui_tree_mode mode)
{
	clear();
	m_mode = mode;
}

void qtvUITreeBase::add_channel(qtvMuxChannel* channel)
{
	clear();
	if (channel)
	{
		QTreeWidgetItem* newch = new QTreeWidgetItem(this);
		newch->setText(0, channel->_name());
		newch->setFlags(getItemFlags(m_mode));
		if (getItemFlags(m_mode) & Qt::ItemIsUserCheckable)
			newch->setCheckState(0, Qt::Checked);
		newch->setData(0, Qt::UserRole, QVariant::fromValue<qtvMuxItem*>(channel));

		for (int32_t i = 0; i < channel->_programs().count(); i++)
		{
			qtvMuxProgram* program = channel->_programs().at(i);
			add_program(program, newch);
		}
		addTopLevelItem(newch);
	}
}

void qtvUITreeBase::add_program(qtvMuxProgram* program, QTreeWidgetItem* chnode)
{
	QTreeWidgetItem* newprog = new QTreeWidgetItem(chnode);
	newprog->setText(0, program->_name());
	newprog->setFlags(getItemFlags(m_mode));
	if (getItemFlags(m_mode) & Qt::ItemIsUserCheckable)
		newprog->setCheckState(0, Qt::Checked);
	newprog->setData(0, Qt::UserRole, QVariant::fromValue<qtvMuxItem*>(program));

	if (m_mode == qui_tree_enumcontext)
	{
		for (int32_t i = 0; i < program->_streams().count(); i++)
		{
			qtvMuxStream* stream = program->_streams().at(i);
			QTreeWidgetItem* newstream = new QTreeWidgetItem(newprog);
			newstream->setText(0, stream->_name());
			newstream->setFlags(getItemFlags(m_mode));
			if (getItemFlags(m_mode) & Qt::ItemIsUserCheckable)
				newstream->setCheckState(0, Qt::Checked);
			newstream->setData(0, Qt::UserRole, QVariant::fromValue<qtvMuxItem*>(stream));
			newprog->addChild(newstream);
		}
	}
	chnode->addChild(newprog);
}

Qt::ItemFlags qtvUITreeBase::getItemFlags(qui_tree_mode mode)
{
	if (mode == qui_tree_enumcontext)
		return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate;
	else return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/* qtvUITreeEnumContext */

qtvUITreeEnumContext::qtvUITreeEnumContext(QWidget* parent) :
	m_channel(NULL),
	m_bvalid(false),
	qtvUITreeBase(qui_tree_enumcontext, parent)
{
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(recvItemChanged(QTreeWidgetItem*, int)));
}

qtvUITreeEnumContext::~qtvUITreeEnumContext()
{
	disconnect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(recvItemChanged(QTreeWidgetItem*, int)));
}

void qtvUITreeEnumContext::setEnumContext(qtvMuxChannel* channel)
{
	resetTree(qui_tree_enumcontext);
	m_bvalid = false;
	add_channel(channel);
	m_channel = channel;
}

bool qtvUITreeEnumContext::checkEnumContextValid()
{
	QHash<uint16_t, uint16_t> m_pids;
	bool bres = false;
	for (int32_t i = 0; i < topLevelItemCount(); i++)
	{
		QTreeWidgetItem* pch = topLevelItem(i);
		if (pch->checkState(0) != Qt::Unchecked)
		{
			for (int32_t j = 0; j < pch->childCount(); j++)
			{
				QTreeWidgetItem* pprog = pch->child(j);
				if (pprog->checkState(0) != Qt::Unchecked)
				{
					qtvMuxProgram* hpprog = qvariant_cast<qtvMuxProgram*>(pprog->data(0, Qt::UserRole));
					if (m_pids[hpprog->_pcr_pid()] != hpprog->_pcr_pid())
						m_pids.insert(hpprog->_pcr_pid(), hpprog->_pcr_pid());
					for (int32_t k = 0; k < pprog->childCount(); k++)
					{
						QTreeWidgetItem* pstream = pprog->child(k);
						if (pstream->checkState(0) != Qt::Unchecked)
						{
							qtvMuxStream* hstream = qvariant_cast<qtvMuxStream*>(pstream->data(0, Qt::UserRole));
							if (m_pids[hstream->_pid()] != hstream->_pid())
								m_pids.insert(hstream->_pid(), hstream->_pid());
						}
					}
				}
			}
		}
	}

	if (m_pids.count() > MUX_FILTER_NAX_NUMS)bres = false;
	else bres = (m_pids.count() != 0);

	if (bres != m_bvalid)
	{
		m_bvalid = bres;
		emit notifyValidChanged(m_bvalid);
	}
	return m_bvalid;
}

vatek_result qtvUITreeEnumContext::getMuxHandle(mux_spec_mode spec, mux_country_code country, qtvMuxServiceChannel** muxhandle)
{
	vatek_result nres = vatek_badstatus;
	qtvMuxServiceChannel* newch = NULL;
	if (checkEnumContextValid())
	{
		nres = qtvMuxServiceChannel::createChannel(&newch);
		if (is_vatek_success(nres))
		{
			nres = newch->resetChannel(spec, country);
			if (!is_vatek_success(nres))delete newch;
		}
	}
		
	if (is_vatek_success(nres))
	{
		for (int32_t i = 0; i < topLevelItemCount(); i++)
		{
			QTreeWidgetItem* pch = topLevelItem(i);
			if (pch->checkState(0) != Qt::Unchecked)
			{
				for (int32_t j = 0; j < pch->childCount(); j++)
				{
					QTreeWidgetItem* pprog = pch->child(j);
					qtvMuxProgram* hpprog = NULL;
					if (pprog->checkState(0) != Qt::Unchecked)
					{
						hpprog = qvariant_cast<qtvMuxProgram*>(pprog->data(0, Qt::UserRole));
						nres = newch->createProgram(hpprog->_pcr_pid(), hpprog->_pmt_pid());

						if (!is_vatek_success(nres))break;

						for (int32_t k = 0; k < pprog->childCount(); k++)
						{
							QTreeWidgetItem* pstream = pprog->child(k);
							if (pstream->checkState(0) != Qt::Unchecked)
							{
								qtvMuxStream* hstream = qvariant_cast<qtvMuxStream*>(pstream->data(0, Qt::UserRole));
								Pmux_stream pmstream = hstream->_raw_stream();
								uint8_t* esbuf = hstream->_esinfo_buffer();
								int32_t eslen = hstream->_esinfo_len();

								if (hstream->_stream_type() == mux_stream_video)
									nres = newch->pushStream(hstream->_pid(), &pmstream->_stream_info.video, esbuf, eslen);
								else if (hstream->_stream_type() == mux_stream_audio)
									nres = newch->pushStream(hstream->_pid(), &pmstream->_stream_info.audio, esbuf, eslen);
								else nres = newch->pushStream(hstream->_pid(), pmstream->stream_type, esbuf, eslen);
							}
							if (!is_vatek_success(nres))break;
						}

						if (is_vatek_success(nres))
							nres = newch->commitProgram();

						if (!is_vatek_success(nres))break;
					}
				}
			}
		}

		if (is_vatek_success(nres))
		{
			newch->finishChannel();
			*muxhandle = newch;
		}
		else delete newch;
	}
	return nres;
}

void qtvUITreeEnumContext::recvItemChanged(QTreeWidgetItem* item, int column)
{
	checkEnumContextValid();
}

/* qtvUITreeContext */

qtvUITreeContext::qtvUITreeContext(QWidget* parent) :
	m_channel(NULL),
	qtvUITreeBase(qui_tree_normal, parent)
{
	
}

qtvUITreeContext::~qtvUITreeContext()
{

}

void qtvUITreeContext::setContext(qtvMuxChannel* channel)
{
	resetTree(qui_tree_normal);
	add_channel(channel);
	m_channel = channel;
}

qtvMuxItem* qtvUITreeContext::getSelectMuxItem()
{
	if (selectedItems().count())
	{
		QTreeWidgetItem* pitem = selectedItems().at(0);
		return qvariant_cast<qtvMuxItem*>(pitem->data(0, Qt::UserRole));
	}
	return NULL;
}