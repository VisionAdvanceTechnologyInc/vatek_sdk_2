#include <codec/ovatek_codec.h>

/* ovatekParamFactory */

ovatekParamFactory::ovatekParamFactory(const uint8_t* def, int32_t size, int32_t advoffect, const Pui_prop_item uiprop, const Pui_prop_item uiadv) :
	m_defparam(def),
	m_size(size),
	m_advoffect(advoffect),
	m_uiprop(uiprop),
	m_uiadv(uiadv)
{

}

ovatekParam* ovatekParamFactory::createParam()
{
	uint8_t* newraw = (uint8_t*)malloc(m_size);
	if (newraw)
	{
		qtvPropertyCollection* newprop = NULL;
		memset(newraw, 0, m_size);
		memcpy(newraw, m_defparam, m_size);

		if (_is_advance())
			newprop = qtvPropertyCollection::createProperties(m_uiprop, newraw, m_uiadv, &newraw[m_advoffect]);
		else newprop = qtvPropertyCollection::createProperties(m_uiprop, newraw);
		return new ovatekParam(this,newraw,newprop);
	}
	return NULL;
}

void ovatekParamFactory::getObsDefault(obs_data_t* settings)
{
	setObsDataWithUIPropsDefault(settings, m_uiprop, m_defparam);
	if (_is_advance())
		setObsDataWithUIPropsDefault(settings, m_uiadv, &m_defparam[m_advoffect]);
}

void ovatekParamFactory::getObsData(obs_data_t* settings, uint8_t* param)
{
	setObsDataWithUIProps(settings, m_uiprop, param);
	if (_is_advance())
		setObsDataWithUIProps(settings, m_uiadv, &param[m_advoffect]);
}

void ovatekParamFactory::setParamObsData(uint8_t* param, obs_data_t* data)
{
	getObsDataWithUIProps(data, m_uiprop, param);
	if (_is_advance())
		getObsDataWithUIProps(data, m_uiadv, &param[m_advoffect]);
}

void ovatekParamFactory::getOBSProperties(obs_properties_t* props, const Pui_prop_item uiprops)
{
	int32_t pos = 0;
	while (uiprops[pos].name)
	{
		if (uiprops[pos].values)
		{
			int32_t idx = 0;
			if (IS_UIPROP_CHECKBOS(uiprops[pos].type))
			{
				while (uiprops[pos].values[idx].name)
				{
					obs_properties_add_bool(props, uiprops[pos].values[idx].name, uiprops[pos].values[idx].name);
					idx++;
				}
			}
			else
			{
				obs_property_t* list;
				list = obs_properties_add_list(props, uiprops[pos].name, uiprops[pos].desc, OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
				while (uiprops[pos].values[idx].name)
				{
					obs_property_list_add_int(list, uiprops[pos].values[idx].name, uiprops[pos].values[idx].val);
					idx++;
				}
			}
		}
		else obs_properties_add_int(props, uiprops[pos].name, uiprops[pos].desc, 0, 0x7FFFFFFF, 1);
		pos++;
	}
}

void ovatekParamFactory::setObsDataWithUIPropsDefault(obs_data_t* settings, const Pui_prop_item uiprops,const uint8_t* raw)
{
	int32_t pos = 0;
	while (uiprops[pos].name)
	{
		uint32_t val = 0;
		vatek_result nres = ui_props_get_uint32(&uiprops[pos], (uint8_t*)raw, &val);
		if (is_vatek_success(nres))
		{
			if (uiprops[pos].values)
			{
				int32_t idx = 0;
				if (IS_UIPROP_CHECKBOS(uiprops[pos].type))
				{
					while (uiprops[pos].values[idx].name)
					{
						bool bval = ((val & uiprops[pos].values[idx].val) != 0);
						obs_data_set_default_bool(settings, uiprops[pos].values[idx].name, bval);
						idx++;
					}
				}
				else obs_data_set_default_int(settings, uiprops[pos].name, val);
			}
			else obs_data_set_default_int(settings, uiprops[pos].name, val);
		}
		pos++;
	}
}

void ovatekParamFactory::setObsDataWithUIProps(obs_data_t* settings, const Pui_prop_item uiprops, const uint8_t* raw)
{
	int32_t pos = 0;

	while (uiprops[pos].name)
	{
		uint32_t val = 0;
		vatek_result nres = ui_props_get_uint32(&uiprops[pos], (uint8_t*)raw, &val);
		if (is_vatek_success(nres))
		{
			if (uiprops[pos].values)
			{
				int32_t idx = 0;
				if (IS_UIPROP_CHECKBOS(uiprops[pos].type))
				{
					while (uiprops[pos].values[idx].name)
					{
						bool bval = ((val & uiprops[pos].values[idx].val) != 0);
						obs_data_set_bool(settings, uiprops[pos].values[idx].name, bval);
						idx++;
					}
				}
				else obs_data_set_int(settings, uiprops[pos].name, val);
			}
			else obs_data_set_int(settings, uiprops[pos].name, val);
		}
		pos++;
	}
}

void ovatekParamFactory::getObsDataWithUIProps(obs_data_t* settings, const Pui_prop_item uiprops, uint8_t* raw)
{
	int32_t pos = 0;
	vatek_result nres = vatek_success;

	while (uiprops[pos].name)
	{
		int32_t isnormal = 0;
		if (uiprops[pos].values)
		{
			int32_t idx = 0;
			if (IS_UIPROP_CHECKBOS(uiprops[pos].type))
			{
				uint32_t val = 0;
				while (uiprops[pos].values[idx].name)
				{
					const char* name = uiprops[pos].values[idx].name;
					if (obs_data_has_user_value(settings, name))
					{
						if (obs_data_get_bool(settings, name))val |= uiprops[pos].values[idx].val;
					}
					idx++;
				}
				nres = ui_props_set_uint32(&uiprops[pos], raw, val);
			}
			else isnormal = 1;
		}
		else isnormal = 1;

		if (isnormal)
		{
			if (obs_data_has_user_value(settings, uiprops[pos].name))
				nres = ui_props_set_uint32(&uiprops[pos], raw, obs_data_get_int(settings, uiprops[pos].name));
		}
		if (!is_vatek_success(nres))blog(LOG_WARNING, "obs data set value fail : [%d:%s]", nres, uiprops[pos].name);
		pos++;
	}
}

/* ovatekParam */

ovatekParam::ovatekParam(ovatekParamFactory* factory, uint8_t* raw, qtvPropertyCollection* properties) :
	m_factory(factory),
	m_rawbuf(raw),
	m_properties(properties)
{
	if(m_factory->_is_advance())
		m_properties->setAdvance(true);
}

ovatekParam::~ovatekParam()
{
	delete m_properties;
	free(m_rawbuf);
}

void ovatekParam::reset()
{
	memcpy(m_rawbuf, m_factory->_default_param(), m_factory->_param_size());
}

obs_data_t* ovatekParam::createObsData()
{
	obs_data_t* newdata = obs_data_create();
	if (newdata)
		m_factory->getObsData(newdata, m_rawbuf);
	else blog(LOG_ERROR, "create obs data fail");
	return newdata;
}

void ovatekParam::setParamObsData(obs_data_t* settings)
{
	m_factory->setParamObsData(m_rawbuf, settings);
}
