/*******************************************************************************
*   (c) 2019 ZondaX GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "gmock/gmock.h"

#include <iostream>
#include <hexutils.h>
#include <lib/parser_txdef.h>
#include <lib/parser.h>
#include "lib/parser_impl.h"

using ::testing::TestWithParam;
using ::testing::Values;

typedef struct {
    uint64_t decimals;
    std::string input;
    std::string expected;
} testcase_bytesPrint_t;

class CompactPrintTests : public ::testing::TestWithParam<testcase_bytesPrint_t> {};

INSTANTIATE_TEST_CASE_P

(
    CompactPrintTestcases,
    CompactPrintTests,
    ::testing::ValuesIn(
        {
            testcase_bytesPrint_t{
                0,
                "0b00204aa9d101",
                "2000000000000"},
            testcase_bytesPrint_t{
                0,
                "57d20a3fce96f1cf8c9cb4378c37a4873f17621ebce404f5aa13",
                "123456789012345678901234567890123456789012345678901234567890"},
            testcase_bytesPrint_t{
                12,
                "04",
                "0.000000000001"},
            testcase_bytesPrint_t{
                1,
                "04",
                "0.1"},
            testcase_bytesPrint_t{
                1,
                "28",
                "1.0"},
            testcase_bytesPrint_t{
                2,
                "28",
                "0.10"},
            testcase_bytesPrint_t{
                0,
                "28",
                "10"},
            testcase_bytesPrint_t{
                12,
                "02890700",
                "0.000000123456"},
            testcase_bytesPrint_t{
                12,
                "131581e97df4102211",
                "1234567.890123456789"},
        })
);

// Parse Compact numbers, reproduce/fix known bug
TEST_P(CompactPrintTests, CompactPrinting) {
    parser_context_t ctx;
    parser_error_t err;

    uint8_t buffer[100];
    auto bufferLen = parseHexString(GetParam().input.c_str(), buffer);
    parser_init(&ctx, buffer, bufferLen);

    compactInt_t cvalue;

    err = _readCompactInt(&ctx, &cvalue);
    EXPECT_EQ(err, parser_ok) << parser_getErrorDescription(err);

    char tmpOut[100];
    uint8_t dummy;
    err = _toStringCompactInt(&cvalue, GetParam().decimals, tmpOut, 100, 0, &dummy);
    EXPECT_EQ(err, parser_ok) << parser_getErrorDescription(err);

    EXPECT_EQ(std::string(tmpOut), GetParam().expected);
}
