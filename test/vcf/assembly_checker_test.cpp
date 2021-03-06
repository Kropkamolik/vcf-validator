/**
 * Copyright 2018 EMBL - European Bioinformatics Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <set>
#include <string>

#include "catch/catch.hpp"
#include "vcf/assembly_checker.hpp"
#include "vcf/error.hpp"
#include "vcf/file_structure.hpp"
#include "vcf/normalizer.hpp"


namespace ebi
{
    TEST_CASE("Sequence comparison", "[assembly_checker]")
    {
        SECTION("Check sequence matches")
        {
            std::string line{"1\t10177\trs367896724\tA\tAC"};
            std::string chromosome{"1"};
            size_t line_num = 1;
            size_t position = 10177;
            std::string reference_allele{"A"};
            std::string alternate_allele{"AC"};

            vcf::RecordCore record_core{line_num, chromosome, position, reference_allele, alternate_allele, vcf::RecordType::NO_VARIATION};
            CHECK(ebi::util::is_matching_sequence(record_core.reference_allele, "A"));
            CHECK_FALSE(ebi::util::is_matching_sequence(record_core.reference_allele, "G"));
        }
    }

    TEST_CASE("Match statistics", "[assembly_checker]")
    {
        SECTION("All matches")
        {
            ebi::vcf::MatchStats match_stats;
            match_stats.add_match_result(true);
            match_stats.add_match_result(true);
            CHECK(match_stats.is_assembly_match());
        }

        SECTION("Mismatches")
        {
            ebi::vcf::MatchStats match_stats;
            match_stats.add_match_result(true);
            match_stats.add_match_result(false);
            CHECK_FALSE(match_stats.is_assembly_match());
        }
    }
}
