#ifndef QINTERNAL_MEDIA_H
#define QINTERNAL_MEDIA_H

#include <core/qtv_media.h>
#include "qinternal_properties.h"

class qi_codec_properties : public qtvCodecProperties, public qtvPropertyHandler
{
public:
	explicit qi_codec_properties(qtvCodecProvider* provider);

	/* qtvPropertiesCollection */
	void resetProperties() override;
	/* qtvCodecProperties */
	qtvCodec* getCodec() override { return m_codec; }

	/* qtvPropertyHandler */
	void raisePropertiesChanged() override;
	void raisePropertyEdited(qtvProperty* prop) override;

protected:
	qtvCodec* m_codec;
	qtvCodecProvider* m_provider;

	/* qi_properties_base */
	void refreshProperties() override;
	void insertProperty(const Pui_prop_item uiprop, uint8_t* raw) override;
};



#endif
