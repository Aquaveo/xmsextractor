"""
XMSExtractor Conanfile and Support
"""
import os
from conans import ConanFile, CMake, tools
from conans.errors import ConanException


class XmsextractorConan(ConanFile):
    """XMSExtractor Conanfile"""
    license = "BSD2"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "wchar_t": ['builtin', 'typedef'],
        "pybind": [True, False],
        "testing": [True, False],
    }
    default_options = {
        'wchar_t': 'builtin',
        'pybind': False,
        'testing': False,
    }
    generators = "cmake", "txt"
    build_requires = "cxxtest/4.4@aquaveo/stable"
    url = "https://github.com/Aquaveo/xmsextractor"
    description = "Extractor library for XMS products"
    exports = "CMakeLists.txt", "LICENSE", "test_files/*"
    exports_sources = "xmsextractor/*", "test_files/*", "_package/*"

    def config_options(self):
        """
        Configure the options for the conan class.
        """
        if self.settings.compiler != 'Visual Studio':
            del self.options.wchar_t

    def set_name(self):
        """
        The function that sets the name of the conan package.
        """
        self.name = 'xmsextractor'

    def configure(self):
        # Set version dynamically using XMS_VERSION env variable.
        self.version = self.env.get('XMS_VERSION', '99.99.99')

        # Raise ConanExceptions for Unsupported Versions
        s_os = self.settings.os
        s_compiler = self.settings.compiler
        s_compiler_version = self.settings.compiler.version

        if s_compiler == "apple-clang" and s_os == 'Linux':
            raise ConanException("Clang on Linux is not supported.")

        if s_compiler == "gcc" and float(s_compiler_version.value) < 5.0:
            raise ConanException("GCC < 5.0 is not supported.")

        if s_compiler == "apple-clang" and s_os == 'Macos' \
                and float(s_compiler_version.value) < 9.0:
            raise ConanException("Clang > 9.0 is required for Mac.")

        if s_compiler == 'Visual Studio' and self.options.wchar_t == 'typedef' and self.options.pybind:
            raise ConanException("wchar_t=typedef not supported with pybind=True")

        self.options['xmscore'].pybind = self.options.pybind
        self.options['xmscore'].testing = self.options.testing

        self.options['xmsinterp'].pybind = self.options.pybind
        self.options['xmsinterp'].testing = self.options.testing

        self.options['xmsgrid'].pybind = self.options.pybind
        self.options['xmsgrid'].testing = self.options.testing

        if s_compiler == 'Visual Studio':
            self.options['xmscore'].wchar_t = self.options.wchar_t
            self.options['xmsinterp'].wchar_t = self.options.wchar_t
            self.options['xmsgrid'].wchar_t = self.options.wchar_t
            self.options['boost'].wchar_t = self.options.wchar_t

    def build(self):
        """
        The build method for the conan class.
        """
        self.output.info("----- RUNNING BUILD()")
        cmake = CMake(self)

        # CxxTest doesn't play nice with PyBind. Also, it would be nice to not
        # have tests in release code. Thus, if we want to run tests, we will
        # build a test version (without python), run the tests, and then (on
        # success) rebuild the library without tests.
        cmake.definitions["IS_PYTHON_BUILD"] = self.options.pybind
        cmake.definitions["BUILD_TESTING"] = self.options.testing
        cmake.definitions["XMS_TEST_PATH"] = "test_files"

        # Version Info
        cmake.definitions["XMS_VERSION"] = '{}'.format(self.version)
        cmake.definitions["PYTHON_TARGET_VERSION"] = self.env.get("PYTHON_TARGET_VERSION", "3.6")

        if self.settings.compiler == 'Visual Studio':
            cmake.definitions["USE_NATIVE_WCHAR_T"] = (self.options.wchar_t == 'builtin')

        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

        # Run the tests if it is testing configuration.
        if self.options.testing:
            self.run_cxx_tests(cmake)

        # If this build is python run the python tests.
        elif self.options.pybind:
            self.run_python_tests_and_upload()

    def package(self):
        """
        The package method of the conan class.
        """
        self.output.info("----- RUNNING PACKAGE()")
        self.copy("license", dst="licenses", ignore_case=True, keep_path=False)

    def package_info(self):
        """
        The package_info method of the conan class.
        """
        self.output.info("----- RUNNING PACKAGE_INFO()")
        self.env_info.PYTHONPATH.append(os.path.join(self.package_folder, "_package"))
        if self.settings.build_type == 'Debug':
            self.cpp_info.libs = [f'{self.name}lib_d']
        else:
            self.cpp_info.libs = [f'{self.name}lib']

    def run_cxx_tests(self, cmake):
        """
        A function to run the cxx_tests.
        """
        self.output.info("----- ** RUNNING RUN_CXX_TEST()")
        try:
            cmake.test()
        except ConanException:
            raise
        finally:
            if os.path.isfile("TEST-cxxtest.xml"):
                with open("TEST-cxxtest.xml", "r") as f:
                    for line in f.readlines():
                        no_newline = line.strip('\n')
                        self.output.info(no_newline)

    def run_python_tests_and_upload(self):
        """A method to run the python tests, and upload if this is a release."""
        self.output.info("----- ** RUNNING RUN_PYTHON_TESTS_AND_UPLOAD()")
        with tools.pythonpath(self):

            # Install required packages for python testing.
            packages_to_install = ['numpy', 'wheel']
            if not self.settings.os == "Macos":
                self.run(f'pip install --user {" ".join(packages_to_install)}')
            else:
                self.run(f'pip install {" ".join(packages_to_install)}')

            # Run python tests.
            path_to_python_tests = os.path.join(self.build_folder, '_package', 'tests')
            self.run(f'python -m unittest discover -v -p *_pyt.py -s {path_to_python_tests}',
                     cwd=os.path.join(self.package_folder, "_package"))

            # Create and upload wheel to aquapi if release and windows
            # We are uploading to aquapi here instead of pypi because pypi doesn't accept
            # the type of package 'linux_x86_64 that we want to upload. They only accept
            # manylinux1 as the plat-tag
            is_release = self.env.get("RELEASE_PYTHON", 'False') == 'True'
            is_mac_os = self.settings.os == 'Macos'
            is_gcc_6 = self.settings.os == "Linux" and float(self.settings.compiler.version.value) == 6.0
            is_windows_md = (self.settings.os == "Windows" and str(self.settings.compiler.runtime) == "MD")
            if is_release and (is_mac_os or is_gcc_6 or is_windows_md):
                self.upload_python_package()

    def upload_python_package(self):
        """
        Upload the python package to AQUAPI_URL.
        """
        self.output.info("----- ** RUNNING UPLOAD_PYTHON_PACKAGE()")
        """A method to upload the python package."""
        devpi_url = self.env.get("AQUAPI_URL", 'NO_URL')
        devpi_username = self.env.get("AQUAPI_USERNAME", 'NO_USERNAME')
        devpi_password = self.env.get("AQUAPI_PASSWORD", 'NO_PASSWORD')
        self.run('devpi use {}'.format(devpi_url))
        self.run('devpi login {} --password {}'.format(devpi_username, devpi_password))
        plat_names = {'Windows': 'win_amd64', 'Linux': 'linux_x86_64', "Macos": 'macosx-10.6-intel'}
        self.run('python setup.py bdist_wheel --plat-name={} --dist-dir {}'.format(
            plat_names[str(self.settings.os)],
            os.path.join(self.build_folder, "dist")), cwd=os.path.join(self.package_folder, "_package"))
        self.run('devpi upload --from-dir {}'.format(os.path.join(self.build_folder, "dist")), cwd=".")

    def export_sources(self):
        """
        Specify sources to be exported.
        """
        self.output.info('----- RUNNING EXPORT_SOURCES()')
        self.copy('*', src=f'{self.name}', dst=f'{self.name}')
        self.copy('*', src='_package', dst='_package')

    def export(self):
        """
        Specify files to be exported.
        """
        self.output.info('----- RUNNING EXPORT()')
        self.copy('CMakeLists.txt')
        self.copy('LICENSE')

    def requirements(self):
        """Requirements"""
        self.requires("boost/1.74.0.3@aquaveo/stable")

        if self.options.pybind:
            self.requires("pybind11/2.5.0@aquaveo/testing")

        self.requires("xmscore/5.0.1@aquaveo/stable")
        self.requires("xmsgrid/6.0.0@aquaveo/stable")
        self.requires("xmsinterp/5.0.0@aquaveo/stable")
