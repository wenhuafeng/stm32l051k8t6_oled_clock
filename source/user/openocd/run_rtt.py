import sys
import datetime
import subprocess
import psutil
import signal

global proc

def close_program(program_name):
    for proc in psutil.process_iter(['pid', 'name']):
        # 检查是否为目标程序
        if proc.info['name'] == program_name:
            proc.terminate()

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

    with open('openocd.cfg', 'w') as f:
        if rtt == True:
            f.write(f"{interface}\n{transport}\n{target}\n{speed}\n{init}\n{rtt_setup}\n{rtt_start}\n{rtt_server_start}")
        else:
            f.write(f"{interface}\n{transport}\n{target}\n{speed}\n{init}\n{halt}\n{program}\n{reset}\n{shutdown}")

def run_openocd():
    openocd_exe = 'openocd.exe'
    close_program(openocd_exe)
    # Execute the OpenOCD script
    exec_cmd = f"{openocd_exe} -f openocd.cfg"
    print(f"execute cmd: {exec_cmd}")
    proc = subprocess.Popen(exec_cmd, shell=True)

def run_putty():
    putty_exe = 'putty.exe'
    close_program(putty_exe)
    proc = subprocess.Popen(f'{putty_exe} -telnet -P 8888 127.0.0.1', shell=True)

def signal_handler(sig, frame):
    if proc is not None:
        proc.send_signal(signal.SIGINT)
    close_program('openocd.exe')
    close_program('putty.exe')
    print("program close")
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def main_func():
    try:
        make_openocd_cfg('daplink', True)
        run_openocd()
        run_putty()
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

if __name__ == "__main__":
    start = datetime.datetime.now()
    main_func()
    end = datetime.datetime.now()
    print('run time: %s second' %(end - start))