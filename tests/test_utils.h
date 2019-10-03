/*
Copyright libCellML Contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <libcellml>

std::string resourcePath(const std::string &resourceRelativePath = "");

std::string fileContents(const std::string &fileName);

void printErrors(const libcellml::Validator &v);
void printErrors(const libcellml::Parser &p);
void expectEqualErrors(const std::vector<std::string> &errors, const libcellml::Logger &logger);

libcellml::ModelPtr createModel(const std::string &name = "");
libcellml::ModelPtr createModelWithComponent(const std::string &name = "");

#define EXPECT_EQ_ERRORS(errors, logger) {SCOPED_TRACE("Error occured here");expectEqualErrors(errors, logger);}
