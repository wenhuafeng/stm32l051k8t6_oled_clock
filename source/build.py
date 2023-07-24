#!/usr/bin/env python
import sys
import os
import datetime
import platform
import subprocess
import shutil
import psutil
import argparse

# save file path
target_path = 'user/output'

stm32 = 'stm32l0'
cs32 = 'cs32f030c8t6'
gd32 = 'gd32f303rc'

def gen_jlink_cmdfile(si, speed, loadfile, dev, addr):
    cmdfile = os.path.join(os.path.dirname(os.path.abspath(loadfile)), (dev + '.jlink'))
    print(f"gen jlink commands file: {cmdfile}")

    try:
        with open(cmdfile, 'w') as f:
            f.write(si)
            f.write(speed)
            device = 'device ' + dev
            f.write(f"{device}\nr\nh\nerase\nloadfile {loadfile} {addr}\nr\nms 10\nq\n")
    except IOError as e:
        print(f"IOError: {e}")
        sys.exit(2)

    return cmdfile

def cp_build_file(source, target):
    if not os.path.exists(source):
        print(f"Source file {source} does not exist.")
        return

    assert not os.path.isabs(source)

    try:
        shutil.copy(source, target)
    except IOError as e:
        print("Unable to copy file. %s" % e)
    except:
        print("Unexpected error:", sys.exc_info())

def rm_dir(dir_path):
    try:
        shutil.rmtree(dir_path)
    except OSError as e:
        print("Error:%s:%s" % (dir_path, e.strerror))

def cmd_run(cmd, background=False):
    if background:
        subprocess.Popen(cmd, shell=True)
    else:
        subprocess.run(cmd, shell=True, check=True)

def build(cc_type, dir_path, build_command, build_log, source_file_hex, source_file_bin):
    print(f"cc type = {cc_type}, wait ...")
    rm_dir(dir_path)
    cmd_run(build_command)
    cmd_run(build_log)
    cp_build_file(source_file_hex, target_path)
    cp_build_file(source_file_bin, target_path)

def build_select(parameter):
    build_params = {
        'g': ('gcc', \
              './build', \
              'make clean', \
              'make -j8', \
              'build/system.hex', \
              'build/system.bin'),
        'ms': ('mdk', \
               './stm32l051k8t6/MDK-ARM/system', \
               'D:/Keil_v5/UV4/UV4.exe -j0 -r ./stm32l051k8t6/MDK-ARM/system.uvprojx -o build_log.txt', \
               'cat ./stm32l051k8t6/MDK-ARM/build_log.txt', \
               'stm32l051k8t6/MDK-ARM/system/system.hex', \
               'stm32l051k8t6/MDK-ARM/system/system.bin'),
        'mc': ('mdk', \
               './cs32f030c8t6/MDK-ARM/system', \
               'D:/Keil_v5/UV4/UV4.exe -j0 -r ./cs32f030c8t6/MDK-ARM/system.uvprojx -o build_log.txt', \
               'cat ./cs32f030c8t6/MDK-ARM/build_log.txt', \
               'cs32f030c8t6/MDK-ARM/system/system.hex', \
               'cs32f030c8t6/MDK-ARM/system/system.bin'),
        'mg': ('mdk', \
               './gd32f303rbt6/MDK-ARM/system', \
               'D:/Keil_v5/UV4/UV4.exe -j0 -r ./gd32f303rbt6/MDK-ARM/system.uvprojx -o build_log.txt', \
               'cat ./gd32f303rbt6/MDK-ARM/build_log.txt', \
               'gd32f303rbt6/MDK-ARM/system/system.hex', \
               'gd32f303rbt6/MDK-ARM/system/system.bin'),
    }

    params = build_params.get(parameter)
    if params:
        build(*params)
    else:
        print("input parameter error!")

def jlink_run(loadfile, dev):
    si = 'si 1\n'
    speed_cmd = 'speed 4000\n'
    address = '0x8000000'

    host_os = platform.system()
    print("host os: %s" % host_os)
    jlink_cmdfile = gen_jlink_cmdfile(si, speed_cmd, loadfile, dev, address)

    jlink_exe_map = {
        'Windows': "Jlink.exe",
        'Linux': "JLinkExe"
    }

    jlink_exe = jlink_exe_map.get(host_os)
    if jlink_exe is None:
        print("Unsupported platform")
        sys.exit(2)

    if host_os == 'Windows' and dev not in [cs32, stm32, gd32]:
        print("Device chip error!")

    exec_cmd = "%s -CommanderScript %s" % (jlink_exe, jlink_cmdfile)
    print("execute cmd: %s" % exec_cmd)
    subprocess.call(exec_cmd, shell=True)

def make_openocd_cfg(if_type, rtt):
    if if_type == 'daplink':
        interface = 'source [find interface/cmsis-dap.cfg]'
    elif if_type == 'stlink':
        interface = 'source [find interface/stlink.cfg]'
    transport = 'transport select swd'
    target = 'source [find target/stm32l0.cfg]'
    speed = 'adapter speed 1000'
    init = 'init'
    halt = 'halt'
    program = 'program user/output/system.bin 0x8000000'
    reset = 'reset'
    shutdown = 'shutdown'

    rtt_setup = 'rtt setup 0x20000000 0x2000 "SEGGER RTT"'
    rtt_start = 'rtt start'
    rtt_server_start = 'rtt server start 8888 0'

    with open('user/openocd/openocd.cfg', 'w') as f:
        if rtt == True:
            f.write(f"{interface}\n{target}\n{speed}\n{init}\n{rtt_setup}\n{rtt_start}\n{rtt_server_start}")
        else:
            f.write(f"{interface}\n{target}\n{speed}\n{init}\n{halt}\n{program}\n{reset}\n{shutdown}")

def close_program(program_name):
    for proc in psutil.process_iter(['pid', 'name']):
        if proc.info['name'] == program_name:
            proc.terminate()

def run_openocd():
    openocd_exe = 'openocd.exe'
    close_program(openocd_exe)
    # Execute the OpenOCD script
    exec_cmd = f"{openocd_exe} -f user/openocd/openocd.cfg"
    print(f"execute cmd: {exec_cmd}")
    subprocess.Popen(exec_cmd, shell=True)

def run_putty():
    putty_exe = 'putty.exe'
    close_program(putty_exe)
    subprocess.Popen(f'{putty_exe} -telnet -P 8888 127.0.0.1', shell=True)

def link_run(if_type, rtt):
    make_openocd_cfg(if_type, rtt)
    run_openocd()

def download_select(parameter):
    function_map = {
        'js': lambda: jlink_run('user/output/system.hex', stm32),
        'jc': lambda: jlink_run('user/output/system.hex', cs32),
        'jg': lambda: jlink_run('user/output/system.hex', gd32),
        'daplink' : lambda: link_run('daplink', False),
        'stlink' : lambda: link_run('stlink', False),
        'daplink_rtt' : lambda: link_run('daplink', True),
        'stlink_rtt' : lambda: link_run('stlink', True)
    }

    func = function_map.get(parameter)
    if func:
        func()
    else:
        print("input parameter error!")

def checksum_file(path):
    current_dir = os.getcwd()
    os.chdir(path)
    subprocess.run(['checksum.exe', 'system.bin', 'CRC32'])
    os.chdir(current_dir)

def common(parameter):
    build_params = ['g', 'ms', 'mc', 'mg']
    download_params = ['js', 'jc', 'jg', 'daplink', 'stlink', 'daplink_rtt', 'stlink_rtt']

    try:
        if parameter in build_params:
            build_select(parameter)
            print("\r\n")
            checksum_file(target_path)
        elif parameter in download_params:
            download_select(parameter)
            if parameter == 'daplink_rtt' or parameter == 'stlink_rtt':
                run_putty()
        else:
            print("parameter error!")
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description='Build script.')
    parser.add_argument('parameter', help='The parameter for the build script.')
    args = parser.parse_args()

    start = datetime.datetime.now()
    common(args.parameter)
    end = datetime.datetime.now()
    print('run time: %s second' %(end - start))

if __name__ == "__main__":
    main()