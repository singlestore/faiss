###################################################################################################################################
from __future__ import print_function
from setuptools import setup, find_packages
from distutils.spawn import find_executable
from distutils.sysconfig import get_python_inc
from numpy import get_include

import os
import shutil
import platform

## Setup files.
platform_type = platform.system()
if platform_type == "Darwin":
    lib_extension="dylib"
elif platform_type == "Linux":
    lib_extension="so"
else:
    print("Unknown platform type: {}".format(platform_type))
    exit(1)

here = os.path.abspath(os.path.dirname(__file__))
makefile = os.path.join(here,"makefile.inc")
python_path = os.path.abspath(find_executable("python"))
python_bin_folder = os.path.dirname(python_path)
python_env = os.path.dirname(python_bin_folder)
python_inc = get_python_inc()
numpy_inc = get_include()

if platform_type == "Darwin":
    lib_extension = "dylib"
    makefile_str = """CXX=/usr/local/opt/llvm/bin/clang++
    CFLAGS=-fPIC -m64 -Wall -g -O3 -msse4 -mpopcnt -fopenmp -Wno-sign-compare -I/usr/local/opt/llvm/include
    CXXFLAGS=$(CFLAGS) -std=c++11
    LLVM_VERSION_PATH=$(shell ls -rt /usr/local/Cellar/llvm/ | tail -n1)
    LDFLAGS=-g -fPIC -fopenmp -L/usr/local/opt/llvm/lib -L/usr/local/Cellar/llvm/${LLVM_VERSION_PATH}/lib
    
    
    # common mac flags
    SHAREDEXT=dylib
    SHAREDFLAGS=-Wl,-F. -bundle -undefined dynamic_lookup
    FAISSSHAREDFLAGS=-dynamiclib
    
    ## MKL
    MKLROOT={}
    BLASLDFLAGS=-Wl,--no-as-needed -L$(MKLROOT)/lib   -lmkl_intel_ilp64 -lmkl_core -lmkl_gnu_thread -ldl -lpthread
    BLASCFLAGS=-DFINTEGER=long
    
    ## Python and SWIG
    SWIGEXEC=/usr/local/bin/swig
    PYTHONCFLAGS=-I{} -I{}    
    """.format(python_env,python_inc,numpy_inc)

    with open(makefile, 'w') as outfile:
        outfile.write(makefile_str)
        outfile.close()

elif platform_type == "Linux":
    gcc_compiler= os.path.abspath(find_executable("g++"))
    lib_extension = "so"

    ## Creating include file.
    with open(makefile, 'w') as outfile:
        outfile.write("### Automatically created make include file.\n")
        outfile.write("CC={}\n".format(gcc_compiler))
        outfile.write("CFLAGS=-fPIC -m64 -Wall -g -O3 -mavx -msse4 -mpopcnt -fopenmp -Wno-sign-compare -std=c++11 -fopenmp\n")
        outfile.write("LDFLAGS=-g -fPIC  -fopenmp\n")
        outfile.write("SHAREDEXT=so\n")
        outfile.write("SHAREDFLAGS=-shared\n")
        outfile.write("FAISSSHAREDFLAGS=-shared\n\n\n")
        outfile.write("MKLROOT={}\n".format(python_env))
        outfile.write("BLASLDFLAGS=-Wl,--no-as-needed -L$(MKLROOT)/lib   -lmkl_intel_ilp64 -lmkl_core -lmkl_gnu_thread -ldl -lpthread\n")
        outfile.write("BLASCFLAGS=-DFINTEGER=long\n\n\n")
        outfile.write("PYTHONCFLAGS=-I{} -I{}\n".format(python_inc,numpy_inc))
        outfile.close()
else:
    print("Unknown platform {}".format(platform_type))


## Run make
os.system("make")
os.system("make py")


## Checking compilation.
check_fpath = os.path.join("python", "_swigfaiss.so")
if not os.path.exists(check_fpath):
    print("Could not find {}".format(check_fpath))
    print("Have you run `make` and `make py` "
          "(and optionally `cd gpu && make && make py && cd ..`)?")

# make the faiss python package dir
shutil.rmtree("faiss", ignore_errors=True)
os.mkdir("faiss")
shutil.copyfile("python/__init__.py", "faiss/__init__.py")
shutil.copyfile("faiss.py", "faiss/faiss.py")
shutil.copyfile("python/swigfaiss.py", "faiss/swigfaiss.py")
shutil.copyfile("python/_swigfaiss.{}".format(lib_extension), "faiss/_swigfaiss.{}".format(lib_extension))
try:
    shutil.copyfile("python/_swigfaiss_gpu.so", "faiss/_swigfaiss_gpu.so")
    shutil.copyfile("python/swigfaiss_gpu.py", "faiss/swigfaiss_gpu.py")
except:
    pass

long_description="""
Faiss is a library for efficient similarity search and clustering of dense 
vectors. It contains algorithms that search in sets of vectors of any size,
 up to ones that possibly do not fit in RAM. It also contains supporting 
code for evaluation and parameter tuning. Faiss is written in C++ with 
complete wrappers for Python/numpy. Some of the most useful algorithms 
are implemented on the GPU. It is developed by Facebook AI Research.
"""
setup(
    name='faiss',
    version='0.1',
    description='A library for efficient similarity search and clustering of dense vectors',
    long_description=long_description,
    url='https://github.com/facebookresearch/faiss',
    author='Matthijs Douze, Jeff Johnson, Herve Jegou',
    author_email='matthijs@fb.com',
    license='BSD',
    keywords='search nearest neighbors',

    install_requires=['numpy'],
    packages=['faiss'],
    package_data={
        'faiss': ['*.{}'.format(lib_extension)],
    },

)
