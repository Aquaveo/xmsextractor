xmsextractor {#mainpage}
============
\tableofcontents

xmsextractor {#xmsextractor}
============

Introduction {#XmsextractorIntroduction}
------------

xmsextractor is a cross-platform C++ software library written by [Aquaveo](http://www.aquaveo.com/) in partnership with [ERDC](http://www.erdc.usace.army.mil/). The code can be obtained by authorized users from [a GIT repository](https://public.git.erdc.dren.mil/computational-analysis-and-mechanics/Filigree) hosted by ERDC.

<table align="center" border="0">
  <tr>
    <td>![](Aquaveo_Logo.png)</td>
  </tr>
</table>

License {#XmsextractorLicense}
-------

The code is distributed under FreeBSD License (See accompanying file LICENSE or [https://aquaveo.com/bsd/license.txt](https://aquaveo.com/bsd/license.txt)). 

Python
------

This library is available as a python module. See [Python Documentation](./pydocs)

Features {#XmsextractorFeatures}
--------
xmsextractor includes the following functionality:

* Polyline scalar extraction from any XmGrid.
* Location scalar extraction from any XmGrid.
* Polyline extraction locations can be obtained.
* XmGrids may have scalars assigned to either the points or to the cells.

The following tutorials are available:

* [Extractor tutorial](Extractor_Tutorial.md)

Testing {#XmsextractorTesting}
-------

The code has numerous unit tests which use the [CxxTest](http://cxxtest.com/) framework. A good way to see how to use the code is to look at the unit tests. Unit tests are located at the bottom of .cpp files within a "#if CXX_TEST" code block. Header files that are named with ".t.h" contain the test suite class definitions.

The Code {#XmsextractorTheCode}
--------
### Namespaces {#XmsextractorNamespaces}
* "xms" - Most code is in this namespace. The use of other namespaces is kept to a minimum. Two-letter prefixes are used as "pseudo-namespaces" in code modules. Preprocessor macros typically start with "XM_" to prevent name collisions.
* "xmt" - Testing code will be put into this namespace once CXX_TEST is upgraded such that it will find the testing code in this namespace.

### Interface pattern {#XmsextractorInterfacePattern}
Many classes follow the interface pattern. An abstract base class is used to define the interface and a concrete implementation class is used to implement the functionality. The implementation class will be named the same as the interface class but will end in "Impl" and will only be found in the .cpp file. For example: xms::InterpIdw and xms::InterpIdwImpl. The Doxygen documentation will be for the Impl class.

More about the interface pattern can be found at the following resources:
* The C++ Programming Language, Stroustroup, p 318 - 322
* Effective C++ Third Edition, Meyers, items 31 and 40
* More Effective C++, Meyers, item 33
* Large-Scale C++ Software Design, Lakos, 6.4.1

"xmsextractor" Name {#xmsextractorName}
------------
The name "xmsextractor" comes from the "XMS" (GMS, SMS, WMS) water modeling software created by Aquaveo.
