Configurator
============
Configurator is a userspace service that creates the database schema (webOS Db8 kinds) for Applications. It scans the /etc/palm/db/kinds directory to read the App's Db8 schema files to create the db kinds in the Db8.

Dependencies
============
Below are the tools and libraries required to build.

	gcc 4.3
	make (any version)
	pkg-config
	glib-2.0 or later
	openwebos/Db8 libraries (libmojocore, libmojoluna)
	openwebos/luna-service2 3.0.0


Build Instructions
==================
This release is provided for informational purpose only. No build support is provided at this time.



# Copyright and License Information

All content, including all source code files and documentation files in this repository except otherwise noted are: 

 Copyright (c) 2010-2012 Hewlett-Packard Development Company, L.P.

All content, including all source code files and documentation files in this repository except otherwise noted are:
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this content except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
