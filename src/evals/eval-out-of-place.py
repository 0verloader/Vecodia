import sys

def read_file(fname):
    """Reads a binary file and returns its data
    Input(s):
        fname (string): The name of the file
    Output(s):
        The data found in the file (bytearray)
    """
    try:
        with open(fname, 'rb') as f:
            return f.read()
    except:
        print('\033[31mCould not read file: {}\033[0m'.format(fname))
        exit()




def interpreter(commands, old):
    """interprets the commands using only from the old code and the partially reconstructed and the commands"""

    reconstructed = []
    for com in commands:
        if com[0] == 'ADD':
            #mm = len(reconstructed)
            reconstructed.extend(com[2])
            #print(new_[mm:mm+ len(com[2])]==com[2], len(com[2]))
        elif com[0] == 'RUN':
            size = com[1]
            iz = len(reconstructed)
            for i in range(size):
                reconstructed.append(old[iz + i])

        elif com[0] == 'COPY1':
            size = com[1]
            it = com[2]
            iz = len(reconstructed)- it
            for i in range(size):
                reconstructed.append(old[iz + i])

        elif com[0] == 'COPY2':
            size = com[1]
            it = com[2]
            iz = len(reconstructed)+ it
            for i in range(size):
                reconstructed.append(old[iz + i])

        elif com[0] == 'COPY':
            type_ = com[1]
            init = com[2]
            size = com[3]

            if type_ == 0:
                for i in range(size):
                    reconstructed.append(old[init + i])
            elif type_ == 1:
                for i in range(size):
                    reconstructed.append(old[init - i])
            elif type_ == 2:
                for i in range(size):
                    reconstructed.append(reconstructed[init + i])
            elif type_ == 3:
                for i in range(size):
                    reconstructed.append(reconstructed[init - i])

    return bytes(reconstructed)


if __name__ == '__main__':

    f_old = sys.argv[1]
    f_new = sys.argv[2]
    f_delta = sys.argv[3]

    OLD = read_file(f_old)
    NEW = read_file(f_new)
    DELTA = read_file(f_delta)
    
    size_of_update = int.from_bytes(DELTA[0:3], byteorder='little', signed=False)
    singletons = int.from_bytes(DELTA[3:6], byteorder='little', signed=False)
    valid = int.from_bytes(DELTA[6:9], byteorder='little', signed=False)

    # print(size_of_update, singletons, valid)

    LL = 9
    MM = 9 + valid
    i = MM +singletons

    coms = 0
    comms = []
    curr_s = 0
    while(curr_s < size_of_update):

        pico = (DELTA[LL]  >> coms) & 1
        if (pico == 1):
            c = ['ADD', 1, [DELTA[MM]]]
            comms.append(c)
            curr_s += 1
            MM+=1
        
        coms += 1

        if(coms == 8):
            LL += 1
            coms = 0


        first_two = int.from_bytes(DELTA[i:i+2], byteorder='little', signed=False)
        opcode = first_two & 7
        length = first_two >> 3
        #print(opcode,length)
        curr_s += length
        if(opcode == 0):
            c = ['ADD', length, DELTA[i+2:i+2+length]]
            comms.append(c)
            i = i+2+length
        elif(opcode == 5):
            c = ['RUN', length]
            comms.append(c)
            i = i+2
        elif(opcode == 6):
            c = ['COPY1', length,DELTA[i+2]]
            comms.append(c)
            i = i+3

        elif(opcode == 7):
            c = ['COPY2', length, DELTA[i+2]]
            comms.append(c)
            i = i+3
        elif(opcode ==1 or opcode ==2 or opcode ==3 or  opcode ==4):
            m = int.from_bytes(DELTA[i+2:i+5], byteorder='little', signed=False)
            c = ['COPY', opcode-1, m, length]
            comms.append(c)
            i = i+5
        else:
            print("ERROR")


    reconstructed = interpreter(comms, OLD)
    print(reconstructed == NEW)