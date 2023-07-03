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
import filecmp

from test_utils import *

def setup_test(single_case_text):
    change_dir(single_case_text.name, single_case_text)
    return True

def migrate_test(single_case_text):
    ret_file = ""
    ret = call_subprocess(single_case_text.CT_TOOL + " --gen-helper-function --out-root=out", single_case_text)
    installed_header_file_root = os.path.join(os.path.dirname(get_ct_path()), "..", "include", "dpct")
    generated_header_file_root = os.path.join(os.getcwd(), "out", "include", "dpct")

    for path, dirs, files in os.walk(installed_header_file_root):
        for name in files:
            installed_abs_path = os.path.join(path, name)
            relative_path = os.path.relpath(installed_abs_path, installed_header_file_root)
            generated_abs_path = os.path.join(generated_header_file_root, relative_path)
            print("Comparing:")
            print(installed_abs_path, generated_abs_path)
            if not filecmp.cmp(installed_abs_path, generated_abs_path, False):
                return False
    return True

def build_test(single_case_text):
    return True

def run_test(single_case_text):
    return True
