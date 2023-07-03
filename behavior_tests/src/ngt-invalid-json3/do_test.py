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


from test_utils import *

def setup_test(single_case_text):
    change_dir(single_case_text.name, single_case_text)
    return True

def migrate_test(single_case_text):
    data = []
    ret = []
    with open("compile_commands.json", 'r') as f:
        data = f.readlines()
        for line in data:
            line = line.replace("directory_placeholder", os.getcwd().replace("\\", "\\\\"))
            ret.append(line)
    ret.pop(5)
    with open("compile_commands.json", 'w') as f:
        f.writelines(ret)
    call_subprocess(single_case_text.CT_TOOL + ' -p=./ --cuda-include-path=' + single_case_text.include_path, single_case_text)
    return is_sub_string("Expected Key, Flow Entry, or Flow Mapping End", single_case_text.print_text)

def build_test(single_case_text):
    return True

def run_test(single_case_text):
    return True


