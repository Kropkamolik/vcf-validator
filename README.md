# vcf-validator [![Build Status](https://travis-ci.org/EBIvariation/vcf-validator.svg?branch=develop)](https://travis-ci.org/EBIvariation/vcf-validator)

Validator for the Variant Call Format (VCF) implemented using C++11.

It includes all the checks from the vcftools suite, and some more that involve lexical, syntactic and semantic analysis of the VCF input. If any inconsistencies are found, they are classified in one of the following categories:

* Errors: Violations of the VCF specification
* Warnings: An indication that something weird happened (different ploidy in samples from the same species) or a recommendation is not followed (missing meta-data)

Please read the wiki for more details about checks already implemented.

## Build

The easiest way to build vcf-validator is using the Docker image provided with the source code. This will create a binary that can be run in any Linux machine.

1. Install and configure Docker following [their tutorial](https://docs.docker.com/engine/getstarted/).
2. Create the Docker image:
    1. Clone this Git repository
    2. Move to the folder the code was downloaded to
    3. Run `docker build -t my-name/vcf-validator docker/`. You can replace `my-name` with your name of choice.
3. Build the application running `docker run -v ${PWD}:/tmp my-name/vcf-validator`. Replace `my-name` with the name used in the previous step.

The following binaries will be created in the `build/bin` subfolder:

* `vcf_validator`: validation tool
* `vcf_debugulator`: automatic fixing tool
* `test_validator` and derivatives: testing correct behaviour of the tools listed above

## Run

### Validator

vcf-validator only needs an input VCF file to be run. It accepts input in the following ways:

* File path as argument: `vcf_validator -v v4.1 -i /path/to/file.vcf`
* Standard input: `vcf_validator -v v4.1 < /path/to/file.vcf`
* Standard input from pipe: `zcat /path/to/file.vcf.gz | vcf_validator -v v4.1`

The VCF version must be always specified, using the `-v` / `--version`. 

The validation level can be configured using `-l` / `--level`. This parameter is optional and accepts 3 values:

* error: Display only syntax errors
* warning: Display both syntax and semantic, both errors and warnings (default)
* stop: Stop after the first syntax error is found

The validation report can be exported in several ways with the `-r` / `--report` option. Several ones may be specified in the same execution.

* stdout: Write human-readable report to the standard output (default)
* database: Write structured report to a database file. The database engine used is SQLite3, so the results can be inspected manually, but they are intended to be consumed by other applications.

The reports written into a file are named after the input file, followed by a timestamp. The default output directory is the same as the input file's if provided using `-i`, or the current directory if using the standard input; it can be changed with the `-o` / `--outdir` option.

### Debugulator

There are some simple errors that can be automatically fixed. The most common error is the presence of duplicate variants. The needed parameters are the original VCF and the report generated by a previous run of the vcf_validator with the option `-r database`.
 
The fixed VCF will be written into the standard output, which you can redirect to a file, or use the `-o` / `--output` option and specify the desired file name.

The logs about what the debugulator is doing will be written into the error output. The logs may be redirected to a log file `2>debugulator_log.txt` or completely discarded ` 2>/dev/null`.

### Examples

Simple example: `vcf_validator -i /path/to/file.vcf -v v4.1`  
Full example: `vcf_validator -i /path/to/file.vcf -v v4.1 -l stop -r database,stdout -o /path/to/output/folder/`

Debugulator example:
```
vcf_validator -i /path/to/file.vcf -v v4.1 -r database -o /path/to/write/report/
vcf_debugulator -i /path/to/file.vcf -e /path/to/write/report/vcf.errors.timestamp.db -o /path/to/fixed.vcf 2>debugulator_log.txt
```

## Developers build

**Note:** Please ignore this section if you only want to use the application.

The end-users build is perfectly valid during development to generate a static binary. Please follow the instructions below if you would like to generate a dynamically linked binary.

### Dependencies

#### Boost

The dependencies are the Boost library core, and its submodules: Boost.filesystem, Boost.program_options, Boost.regex and Boost.system.
If you are using Ubuntu, the required packages' names will be `libboost-dev`, `libboost-filesystem-dev`, `libboost-program-options-dev` and `libboost-regex-dev`.

#### ODB

You will need to download the ODB compiler, the ODB common runtime library, and the SQLite database runtime library from [this page](http://codesynthesis.com/products/odb/download.xhtml).

ODB requires SQLite3 to be installed. If you are using Ubuntu, the required packages' names will be `libsqlite3-0` and `libsqlite3-dev`.

To install the ODB compiler, the easiest way is to download the `.deb` or `.rpm` packages, in order to be installed automatically with `dpkg`. Both the ODB runtime and SQLite database runtime libraries can be installed manually running `./configure && make && sudo make install`. This will install the libraries in `/usr/local/lib`.

If you don't have root permissions, please run `./configure --prefix=/path/to/odb/libraries/folder` to specify which folder to install ODB in, then `make && make install`, without `sudo`.

### Compile

The build has been tested on the following compilers:
* Clang 3.5 to 3.7
* GCC 4.8 to 5.0

In order to create the build scripts, please run `cmake` with your preferred generator. For instance, `cmake -G "Unix Makefiles"` will create Makefiles, and to build the binaries, you will need to run `make`. If the ODB libraries were not found during the build, please run `sudo updatedb && sudo ldconfig`.

For those users who need static linkage, the option `-DBUILD_STATIC=1` must be provided to the `cmake` command. Also, if ODB has been installed in a non-default location, the option `-DODB_PATH=/path/to/odb/libraries/folder` must be also provided to the `cmake` command.

In any case, the following binaries will be created in the `bin` subfolder:

* `vcf_validator`: validation tool
* `vcf_debugulator`: automatic fixing tool
* `test_validator` and derivatives: testing correct behaviour of the tools listed above

### Test

Unit tests can be run using the binary `bin/test_validator` or, if the generator supports it, a command like `make test`. The first option may provide a more detailed output in case of test failure.

**Note**: Tests that require input files will only work when executed with `make test` or running the binary from the project root folder (not the `bin` subfolder).

### Generate code from descriptors

Code generated from descriptors shall be always up-to-date in the GitHub repository. If changes to the source descriptors were necessary, please generate the Ragel machines C code from `.ragel` files using:

```
ragel -G2 src/vcf/vcf_v41.ragel -o inc/vcf/validator_detail_v41.hpp
ragel -G2 src/vcf/vcf_v42.ragel -o inc/vcf/validator_detail_v42.hpp
ragel -G2 src/vcf/vcf_v43.ragel -o inc/vcf/validator_detail_v43.hpp
```

And the full ODB-based code from the classes definitions using:

```
odb --include-prefix vcf --std c++11 -d sqlite --generate-query --generate-schema --hxx-suffix .hpp --ixx-suffix .ipp --cxx-suffix .cpp --output-dir inc/vcf/ inc/vcf/error.hpp
mv inc/vcf/error-odb.cpp src/vcf/error-odb.cpp
```
