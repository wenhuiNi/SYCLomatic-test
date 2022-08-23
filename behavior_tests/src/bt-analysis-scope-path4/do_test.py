# ====------ do_test.py---------- *- Python -* ----===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#
# ===----------------------------------------------------------------------===#
import subprocess
import platform
import os
import sys
import shutil
import glob

from test_utils import *


def setup_test():
    change_dir(test_config.current_test)
    if os.path.exists("cuda_symlink"):
        os.unlink("cuda_symlink")
    os.symlink("cuda", "cuda_symlink")
    return True


def migrate_test():
    # clean previous migration output
    if (os.path.exists("out")):
        shutil.rmtree("out")

    migrate_cmd = test_config.CT_TOOL + " --cuda-include-path=" + test_config.include_path + " " + os.path.join(
        "cuda",
        "call_device_func_outside.cu") + " --in-root=cuda" + " --out-root=out"

    call_subprocess(migrate_cmd + " --analysis-scope-path=cuda_symlink")
    if (not os.path.exists(
            os.path.join("out", "call_device_func_outside.dp.cpp"))):
        return False
    return True


def build_test():
    return True


def run_test():
    return True