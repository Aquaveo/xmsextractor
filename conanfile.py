"""
conanfile.py for the xmsextractor project.
"""
from xmsconan.xms_conan_file import XmsConanFile


class XmsextractorConan(XmsConanFile):
    """
    XmsextractorConan class used for defining the conan info.
    """
    name = 'xmsextractor'
    url = 'https://github.com/Aquaveo/xmsextractor'
    description = 'Extractor library for XMS products'
    xms_dependencies = [
        'xmscore/[>=6.0.1 <7.0.0]@aquaveo/stable',
        'xmsgrid/[>=7.0.1 <9.0.0]@aquaveo/stable',
        'xmsinterp/[>=6.0.1 <7.0.0]@aquaveo/stable',
    ]
    extra_export_sources = [
        'test_files'
    ]


LIBRARY_NAME = XmsextractorConan.name
