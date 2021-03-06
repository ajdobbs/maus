/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>

#include "gtest/gtest.h"

#include "src/common_cpp/JsonCppProcessors/JobHeaderProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/RunHeaderProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/RunFooterProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/JobFooterProcessor.hh"
#include "tests/cpp_unit/JsonCppProcessors/CommonProcessorTest.hh"

// Test the headers and footers (JobHeader, JobFooter, RunHeader, RunFooter)...

// Rogers: critique is that we never check that all data was filled; so, for
//         example, if we never touch "energy" or something, it never gets used.
//         But checking this is a lot of work...

namespace MAUS {
namespace HeaderProcessorTest {
std::string JOB_HEADER =
  std::string("{")+
  std::string("\"start_of_job\":{\"date_time\":\"1976-04-04T00:00:00.000000\"}, ")+
  std::string("\"bzr_configuration\":\"\", ")+
  std::string("\"bzr_revision\":\"\", ")+
  std::string("\"bzr_status\":\"\", ")+
  std::string("\"maus_version\":\"\", ")+
  std::string("\"json_configuration\":\"output cpp root test\", ")+
  std::string("\"maus_event_type\":\"JobHeader\"")+
  std::string("}");

std::string JOB_FOOTER =
  std::string("{")+
  std::string("\"end_of_job\":{\"date_time\":\"1976-04-04T00:00:00.000000\"}, ")+
  std::string("\"maus_event_type\":\"JobFooter\"")+
  std::string("}");

std::string RUN_HEADER =
  std::string("{")+
  std::string("\"run_number\":-7, ")+
  std::string("\"maus_event_type\":\"RunHeader\"")+
  std::string("}");

std::string RUN_FOOTER =
  std::string("{")+
  std::string("\"run_number\":-7, ")+
  std::string("\"maus_event_type\":\"RunFooter\"")+
  std::string("}");


TEST(HeaderFooterProcessorTest, JobHeaderProcessorTest) {
    JobHeaderProcessor proc;
    ProcessorTest::test_value(&proc, JOB_HEADER);
}

TEST(HeaderFooterProcessorTest, JobFooterProcessorTest) {
    JobFooterProcessor proc;
    ProcessorTest::test_value(&proc, JOB_FOOTER);
}

TEST(HeaderFooterProcessorTest, RunHeaderProcessorTest) {
    RunHeaderProcessor proc;
    ProcessorTest::test_value(&proc, RUN_HEADER);
}

TEST(HeaderFooterProcessorTest, RunFooterProcessorTest) {
    RunFooterProcessor proc;
    ProcessorTest::test_value(&proc, RUN_FOOTER);
}
}
}
