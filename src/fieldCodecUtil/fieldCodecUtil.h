#ifndef __FIELD_CODEC_UTIL_H__
#define __FIELD_CODEC_UTIL_H__

#include <cstdint>

const static int endFieldID = -1;

enum Result {
    success,
    failure
};

struct FieldDefinition {
    int fieldId;
    int fieldLen;
};

using byte = uint8_t;

class FieldCodecUtil {
public:
    FieldCodecUtil();
    void useFieldTemplate(FieldDefinition *fieldTemplate, unsigned char *buf);
    void appendFieldTemplate(FieldDefinition *fieldTemplate);
    uint64_t getFieldValue(int fieldId);
    Result setFieldValue(int fieldId, uint64_t fieldValue);
    
private:
    int getFieldTemplateSize(FieldDefinition *fieldTemplate);
    Result checkFieldId(int fieldId);
    int getBitOffsetInCurField(int fieldId);
    int getFieldLenInCurField(int fieldId);
    
    FieldDefinition *curField_m;
    int curBitOffset_m;
    unsigned char *dataBuf_m;
};


#endif