# What's New in Version 1.next


## Database Logging
The SMP application now supports logging of it's data to either SQLITE or a PostgreSQL server.  This extension has necessitated a switch in how the model is run.

The `KTAB_SMP` application has new a "Configure Database" menu and toolbar item, which will prepare the connection to a PostgreSQL database server.

The `smpc` application no longer has the `--dbname` flag, which has been supplanted by a `--connstr` flag. There are two possible parameterizations for the connection string:

- SQLITE: *--connstr "Driver=QSQLITE;Database=database_name"*; note that the ".db" extension must be excluded from the database name
- PostgreSQL: *--connstr "Driver=QPSQL;Server=server_ip_address;Port=port_number;Database=database_name;Uid=database_user_id;Pwd=database_user_password"*; note that the port number is optional and can be excluded if the server listens on the default port.

For PostgreSQL, the database user specified in database_user_name must have "connect" access to the default `postgres` database, as well as the rights to create tables, insert data, and select data on at least one database.  Optionally, the user could have rights to add databases.

There are several new dynamic link libraries which are necessary to support this additional functionality.  These will be included in the KTAB_SMP.zip release archive.


## Compilation of SMP Dynamic Link Library
Compilation of the SMP project now results in a dynamic link library for the SMP model, as well as the existing `smpc` and `KTAB_SMP` applications.  This library can be useful for embedding the SMP model in other applications - even those not built in C++.  On Windows, the library is named `smpDyn.dll`, and on Linux, it is `libsmpDyn.so`.  There is also a new application compiled named `smpcDyn` which uses the library; no change has been made to the existing SMP applications.  Use of the new SMP library will be documented elsewhere.

A few changes need to be made to how some supporting libraries for KTAB are compiled; specifically:

- On Linux, Tinyxml2 must be recompiled with the `-fPIC` flag.  We recommend adding this into that project's `CMakeLists.txt` file by setting the POSITION_INDEPENDENT_CODE property; for example, as in "set_target_properties(tinyxml2_static PROPERTIES COMPILE_DEFINITONS "TINYXML2_EXPORT" VERSION "${GENERIC_LIB_VERSION}" SOVERSION "${GENERIC_LIB_SOVERSION}" POSITION_INDEPENDENT_CODE ON)"

- On both Windows & Linux, easylogging++ must be recompiled with the new CMakeLists.txt file available [here](./easyloggingpp/CMakeLists.txt); please see the [compilation instructions](./easyloggingpp/compiling_elpp.md).


## Enhanced Application Testing ##
We have added two new shell scripts to the KTAB project which automate testing all the applications.

1. [ALL KTAB Applications (ex. SMP)](./KTAB_Test_Apps.sh): This script runs several applications with specified flags, and compares the results against known results.  The user is notified in the case of any nontrivial deviation in application output, or indication of failure.  The applications are:
- demoutils
- demomodel
- leonApp
- mtchApp
- agdemo
- rpdemo
- minwater
- csg
2. [SMP](./examples/smp/SMPC_RefRuns_Compare.sh): This script run the smpc application with four example datasets: `SOE-Pol-Comp`, `dummyData_3Dim`, `dummyData_6Dim`, and `dummyData-a040`. Results are compared against known results with the same datasets and parameters.  The user is notified in the case of any nontrivial deviation in application output, or indication of failure.

The Travis Continual Integration process which runs to validate every Pull Request runs the `SMPC_RefRun_Compare.sh` script and fails if the script fails.  To test the other applications, Travis runs the script `KTAB_Test_Apps_Travis.sh`, which skips `demoutils`, due to an inexplicable error on only the Travis server.
