#define _BSD_SOURCE             /* See feature_test_macros(7) */
#include <endian.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include "fieldCodecUtil.h"

const static int byteLen= 8;

FieldCodecUtil::FieldCodecUtil()
    :curField_m{nullptr}, curBitOffset_m{0}
{

}

void FieldCodecUtil::useFieldTemplate(FieldDefinition *fieldTemplate,
    unsigned char *buf)
{
    curField_m = fieldTemplate;
    curBitOffset_m = 0;
    dataBuf_m = buf;
}

void FieldCodecUtil::appendFieldTemplate(FieldDefinition *fieldTemplate)
{
    curBitOffset_m += getFieldTemplateSize(curField_m);
    curField_m = fieldTemplate;
}

uint64_t FieldCodecUtil::getFieldValue(int fieldId)
{
    if (checkFieldId(fieldId) != success) 
        return -1;

    int fieldLen = getFieldLenInCurField(fieldId);
    int startBitOffset = curBitOffset_m + getBitOffsetInCurField(fieldId);
    int endBitOffset = startBitOffset + fieldLen - 1;
    int startByteOffset = startBitOffset / byteLen;
    int endByteOffset = endBitOffset / byteLen;
    int startBitSubOffset  = startBitOffset % byteLen;
    int endBitSubOffset    = endBitOffset % byteLen;

    uint64_t result = 0;
    if (startByteOffset == endByteOffset) {
        result = dataBuf_m[startByteOffset];
        int lshiftBitNum = 7 - endBitSubOffset;
        result >>= lshiftBitNum;
        result &= ((1 << fieldLen) - 1);
    }
    else {
        uint32_t shiftBuf[2];
        uint32_t *highNum = &shiftBuf[0];
        uint32_t *lowNum = &shiftBuf[1];
        uint8_t *shiftBufPtr = (uint8_t *)shiftBuf;
        for (int i = startByteOffset; i <= endByteOffset; ++i) {
            *shiftBufPtr++ = dataBuf_m[i];
        }

        *highNum = ntohl(*highNum);
        *lowNum = ntohl(*lowNum);
        int alignShiftNum = 64 - (startBitSubOffset + fieldLen);
        if (alignShiftNum < 32) {
            result = (*lowNum >> alignShiftNum) | (*highNum << (32 - alignShiftNum));
        }
        else {
            result = (*highNum >> (alignShiftNum - 32));
        }
        result &= ((uint64_t)-1) >> (32 - fieldLen);
    }

    return result;
}

Result FieldCodecUtil::setFieldValue(int fieldId, uint64_t fieldValue)
{
    if (checkFieldId(fieldId) != success) 
        return failure;

    int fieldLen = getFieldLenInCurField(fieldId);
    int startBitOffset = curBitOffset_m + getBitOffsetInCurField(fieldId);
    int endBitOffset = startBitOffset + fieldLen - 1;
    int startByteOffset = startBitOffset / byteLen;
    int endByteOffset = endBitOffset / byteLen;
    int startBitSubOffset  = startBitOffset % byteLen;
    int endBitSubOffset    = endBitOffset % byteLen;

    if (startByteOffset == endByteOffset) {
        uint8_t value = fieldValue;
        int lshiftBitNum = 7 - endBitSubOffset;
        value <<= lshiftBitNum;
        dataBuf_m[startByteOffset] |= value;
    }
    else  {
        int alignShift = 7 - endBitSubOffset;
        printf("startOffset: %d %d %d, endoffset: %d %d %d, alignShift: %d",
            startBitOffset, startByteOffset, startBitSubOffset, endBitOffset,
            endByteOffset, endBitSubOffset, alignShift);
        byte shiftBuf[8];
        uint32_t *highNum = (uint32_t *)&shiftBuf[0];
        uint32_t *lowNum = (uint32_t *)&shiftBuf[4];
        
        uint32_t tmpLowNum = fieldValue;
        uint32_t tmpHighNum = fieldValue >> 32;
		uint32_t tmpNum = tmpLowNum;
		tmpLowNum  <<= alignShift;
        tmpHighNum <<= alignShift;
		tmpHighNum |= (tmpNum >> (32 - alignShift));

        *lowNum = htonl(tmpLowNum);
		*highNum = htonl(tmpHighNum);

        byte *firstBytePtr = &dataBuf_m[startByteOffset];
        byte *dataPtr = &dataBuf_m[endByteOffset];
        byte tmpData  = *dataPtr;                         /* Get RT most byte */
        tmpData &= (0xff >> (endBitSubOffset+1));        /* Clear area where LSBs will go */
        byte *src = (byte *)&shiftBuf[7];
        *dataPtr-- = tmpData | *src--;               /* OR them in */

        while (dataPtr != firstBytePtr)
        {
                *dataPtr-- = *src--;
        }

        tmpData = *dataPtr;                          /* Get left most byte */
        tmpData &= ~(0xff >> startBitSubOffset);         /* Clear area where MSBs will go */
        *dataPtr = tmpData | *src;                   /* OR them in */
    }

    return success;
}

int FieldCodecUtil::getFieldTemplateSize(FieldDefinition *fieldTemplate)
{
    int result = 0;
    FieldDefinition *fieldPtr = fieldTemplate;
    while (fieldPtr->fieldId != endFieldID) {
        result += fieldPtr->fieldLen;
        ++fieldPtr;
    }

    return result;
}

Result FieldCodecUtil::checkFieldId(int fieldId)
{   
    FieldDefinition *fieldPtr = curField_m;
    while (fieldPtr->fieldId != endFieldID) {
        if (fieldPtr->fieldId == fieldId) 
            return success;
        ++fieldPtr;
    }

    return failure;
}

int FieldCodecUtil::getBitOffsetInCurField(int fieldId)
{
    int result = 0;
    FieldDefinition *fieldPtr = curField_m;
    while (fieldPtr->fieldId != fieldId) {
        result += fieldPtr->fieldLen;
        ++fieldPtr;
    }
    
    return result;
}

int FieldCodecUtil::getFieldLenInCurField(int fieldId)
{
    FieldDefinition *fieldPtr = curField_m;
    while (fieldPtr->fieldId != endFieldID) {
        if (fieldPtr->fieldId == fieldId)
            return fieldPtr->fieldLen;
        ++fieldPtr;
    }
    
    return -1;
}
