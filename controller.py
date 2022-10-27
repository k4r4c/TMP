#!/usr/bin/python

import queue
import threading
import sys, getopt
import subprocess

#Deploy progran to host

class Host (threading.Thread):
    user = 'karak'
    folder = 'tmp'
    time = 0
    def __init__(self,name, host_address, main,inqueue,outqueue):
        threading.Thread.__init__(self)
        self.host = host_address
        self.main = main
        self.name=name
        self.inqueue=inqueue
        self.outqueue=outqueue

    def push(self,program):
        (out,err) = subprocess.Popen("scp {program} {user}@{host}:./home/{user}/{folder}/".format(user=self.user, host=self.host, program=program, folder=self.folder), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        if(err != b''):
            return err.decode("utf-8")
        return out
    def run(self):
        print('Starting', self.host)
        self.time = self.getTime()
#        if(self.name == 'client'):
        self.outqueue.put(self.time)
        self.inqueue.get()
        #if(self.host == 'client')
        print('Exiting', self.host)
#        execute("./home/{user}/{folder}/{cmd}".format(user=user, folder=folder, cmd=self.main))
    
    #Execute progran on host
    def execute(self,cmd):
        (out,err) = subprocess.Popen("ssh {user}@{host} {cmd} ".format(user=self.user, host=self.host, cmd=cmd), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        if(err != b''):
            return err.decode("utf-8")
        return out
    
    #Get file from host
    def get(self,program):
        (out,err) = subprocess.Popen("scp {user}@{host}:./home/{user}/{folder}/ ./".format(user=self.user, host=self.host, program=self.program), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        if(err != b''):
            return err.decode("utf-8")
        return out
 
    def getTime(self):
        (out,err) = subprocess.Popen("ssh {user}@{host} date +%s%N".format(user=self.user, host=self.host), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        if(err != b''):
            return err.decode("utf-8")
        return int(out)

def main(argv):
    clientToServer = queue.Queue()
    serverToClient = queue.Queue()
    try:
        opts, args = getopt.getopt(argv,"hc:s:",["ifile=","ofile="])
    except getopt.GetoptError:
        print('analysis.py -c <clientHost> -s <serverHost>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('analysis.py -c <clientHost> -s <serverHost>')
            sys.exit()
        elif opt in ("-c", "--ifile"):
            client = Host('client',arg, 'main',serverToClient,clientToServer)
        elif opt in ("-s", "--ofile"):
            server = Host('server',arg, 'main',clientToServer,serverToClient)
    return (client,server)

if __name__ == "__main__":
    (client,server) = main(sys.argv[1:])   
    print('Initialized client at address', client.host, 'and server at', server.host,'.')
    client.push('./tool')
    client.start()
    server.start()
    client.join()
    server.join()
    print('Client clock time (ns):',client.time)
    print('idCounter:',client.name)
    print('idCounter:',server.name)
    print('Server clock time (ns):',server.time)
    print('Time difference (absolute value):', abs(client.time - server.time))
    
#    520 562 743
#     43 436 013
#      1 308 508


#    client.push('./tool')
#    client.execute('./tool generate 100')

