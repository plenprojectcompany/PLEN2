import subprocess

cmd = r'"C:\Program Files (x86)\SourceMonitor\SourceMonitor.exe" /C sourcemonitor_commands.xml'
subprocess.call(cmd, shell = True)