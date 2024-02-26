import subprocess
import time

executable_path = './build/scanner'

ip_addres = '194.87.214.70'
begin_port = '1'
end_port = '1000'

start_time = time.time()
subprocess.call([executable_path,ip_addres,str(begin_port),str(end_port)])
end_time = time.time()  
execution_time = end_time - start_time  
 
print(f"Время выполнения программы: {execution_time} секунд")