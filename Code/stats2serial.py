import psutil
import serial
import time

def get_cpu_temperature():
    # This function gets the average CPU temperature across all cores
    temps = psutil.sensors_temperatures()
    if not temps:
        return None
    
    core_temps = []
    for name, entries in temps.items():
        for entry in entries:
            if 'Core' in entry.label:  # Looks for any label with 'Core' in it
                core_temps.append(entry.current)
    
    if core_temps:
        return sum(core_temps) / len(core_temps)
    return None

def write_to_serial_port(temp, cpu, speed):
    # This function writes the temperature to ttyUSB0
    with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:
        ser.write(f"{temp},{cpu},{speed}\n".encode())
        
def get_network_speed(interval=1):
    # Get the initial network I/O counters
    net_io_start = psutil.net_io_counters()
    
    # Wait for the specified interval
    time.sleep(interval)
    
    # Get the network I/O counters again
    net_io_end = psutil.net_io_counters()
    
    # Calculate the upload and download speed (in bytes per second)
    bytes_sent_per_sec = (net_io_end.bytes_sent - net_io_start.bytes_sent) / interval
    bytes_recv_per_sec = (net_io_end.bytes_recv - net_io_start.bytes_recv) / interval
    
    # Convert the speeds to kilobytes per second (optional)
    upload_speed_kbps = bytes_sent_per_sec / 1024
    download_speed_kbps = bytes_recv_per_sec / 1024
    
    return f"{upload_speed_kbps},{download_speed_kbps}"

def main():
  while True:
    temp = get_cpu_temperature()
    if temp is not None:
      cpu = psutil.cpu_percent()
      speed = get_network_speed()
      write_to_serial_port(temp, cpu, speed)
      #time.sleep(1)

if __name__ == "__main__":
    main()
