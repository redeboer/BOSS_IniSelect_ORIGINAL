Configuration folder
====================

The `config` contains folder the configuration files that are loaded at runtime when running a script from the `scripts` / `exe` folder. The configuration files are loaded through a derived object of the `ConfigLoader` class.

The `config` folder has a subdirectory for each script in the `scripts` folder. They are named accordingly. Each of these subdirectories in turn has a subdirectory for the analysis you are performing and, if necessary, a subdirectory for the type of data you are analysising (data, inclusive, or exclusive Monte Carlo).