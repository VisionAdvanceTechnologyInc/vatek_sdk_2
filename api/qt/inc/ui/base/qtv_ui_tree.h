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

#ifndef QTV_UI_TREE_H
#define QTV_UI_TREE_H

#include <QTreeWidget>
#include <core/qtv_mux.h>
#include <ui/base/qtv_ui_core.h>

typedef enum _qui_tree_mode
{
	qui_tree_normal,
	qui_tree_enumcontext,
}qui_tree_mode;

class QTV_DECL qtvUITreeBase : public QTreeWidget
{
	Q_OBJECT
public:
	explicit qtvUITreeBase(qui_tree_mode mode = qui_tree_normal,QWidget* parent = nullptr);
	virtual ~qtvUITreeBase();
protected:
	qui_tree_mode m_mode;

	void resetTree(qui_tree_mode mode);
	void add_channel(qtvMuxChannel* channel);
	void add_program(qtvMuxProgram* program, QTreeWidgetItem* chnode);

	static Qt::ItemFlags getItemFlags(qui_tree_mode mode);
};

class QTV_DECL qtvUITreeEnumContext : public qtvUITreeBase
{
	Q_OBJECT
public:
	explicit qtvUITreeEnumContext(QWidget* parent = nullptr);
	virtual ~qtvUITreeEnumContext();

	virtual void setEnumContext(qtvMuxChannel* channel);
	virtual bool checkEnumContextValid();
	virtual vatek_result getMuxHandle(mux_spec_mode spec,mux_country_code country,qtvMuxServiceChannel** muxhandle);

Q_SIGNALS:
	void notifyValidChanged(bool isvalid);

protected:
	qtvMuxChannel* m_channel;
	bool m_bvalid;

private Q_SLOTS:
	void recvItemChanged(QTreeWidgetItem* item, int column);
};

class QTV_DECL qtvUITreeContext : public qtvUITreeBase
{
	Q_OBJECT
public:
	explicit qtvUITreeContext(QWidget* parent = nullptr);
	virtual ~qtvUITreeContext();

	virtual void setContext(qtvMuxChannel* channel);
	virtual qtvMuxItem* getSelectMuxItem();

protected:
	qtvMuxChannel* m_channel;
};

#endif
