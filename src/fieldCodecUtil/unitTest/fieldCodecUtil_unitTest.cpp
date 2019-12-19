#include "../fieldCodecUtil.h"

#include <gtest/gtest.h>

namespace {

enum FieldType {
    version,
    pf,
    tf,
    mp,
    mesType,
    mesLen,
    teid,
    sn,
    spare1,
    spare2
};

FieldDefinition gtpv2Header[] = {
    {version, 3},
    {pf, 1},
    {tf, 1},
    {mp, 1},
    {spare1, 2},
    {mesType, 8},
    {mesLen, 16},
    {teid, 32},
    {sn, 24},
    {spare2, 8},
    {endFieldID, 0}
};

unsigned char buf[] = { 0x48, 0x20, 0x01, 0x19, 0x00, 0x28, 0x92, 0x9e,
  0x00, 0x58, 0xe9, 0x00, 0x01, 0x00, 0x08, 0x30,
  0x64, 0x00, 0x91, 0x01, 0x00, 0x00, 0x00, 0xf0};

unsigned char setBuf[100];

class FieldCodecUtilTest : public testing::Test {
protected:
    void SetUp() override {
        fieldCodecUtil_m.useFieldTemplate(gtpv2Header, buf);
        setFieldCodecUtil_m.useFieldTemplate(gtpv2Header, setBuf);
        setFieldCodecUtil_m.setFieldValue(version, 2);
        setFieldCodecUtil_m.setFieldValue(pf, 0);
        setFieldCodecUtil_m.setFieldValue(tf, 1);
        setFieldCodecUtil_m.setFieldValue(mp, 0);
        setFieldCodecUtil_m.setFieldValue(mesType, 32);
        setFieldCodecUtil_m.setFieldValue(mesLen, 281);
        setFieldCodecUtil_m.setFieldValue(teid, 2658974);
        setFieldCodecUtil_m.setFieldValue(sn, 22761);
    }

    

    FieldCodecUtil fieldCodecUtil_m;
    FieldCodecUtil setFieldCodecUtil_m;
};

}

TEST_F(FieldCodecUtilTest, GetFiledValue)
{
    EXPECT_EQ(2, fieldCodecUtil_m.getFieldValue(version));
    EXPECT_EQ(0, fieldCodecUtil_m.getFieldValue(pf));
    EXPECT_EQ(1, fieldCodecUtil_m.getFieldValue(tf));
    EXPECT_EQ(0, fieldCodecUtil_m.getFieldValue(mp));
    EXPECT_EQ(32, fieldCodecUtil_m.getFieldValue(mesType));
    EXPECT_EQ(281, fieldCodecUtil_m.getFieldValue(mesLen));
    EXPECT_EQ(2658974, fieldCodecUtil_m.getFieldValue(teid));
    EXPECT_EQ(22761, fieldCodecUtil_m.getFieldValue(sn));
}

TEST_F(FieldCodecUtilTest, SetFiledValue)
{
    EXPECT_EQ(0x48, setBuf[0]);
    EXPECT_EQ(0x20, setBuf[1]);
    EXPECT_EQ(0x01, setBuf[2]);
    EXPECT_EQ(0x19, setBuf[3]);
    EXPECT_EQ(0x00, setBuf[4]);
    EXPECT_EQ(0x28, setBuf[5]);
    EXPECT_EQ(0x92, setBuf[6]);
    EXPECT_EQ(0x9e, setBuf[7]);
    EXPECT_EQ(0x00, setBuf[8]);
    EXPECT_EQ(0x58, setBuf[9]);
    EXPECT_EQ(0xe9, setBuf[10]);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

