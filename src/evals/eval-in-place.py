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




def interpreter(commands, old , lenNew):
    """interprets the commands using only from the old code and the partially reconstructed and the commands"""

    reconstructed = []
    for i in old:
        reconstructed.append(i)

    for i in range(50000):
        reconstructed.append(0)

    lm = 0
    cur = 0
    for com in commands:
        #print(com[0],end =" ") 
        if com[0] == 'ADD': # 0
            for z in range(len(com[2])):
                reconstructed[cur + z] = com[2][z];

            cur += len(com[2])
            #print(len(com[2]))

        elif com[0] == 'RUN': # 5
            size = com[1]
            for i in range(size):
                reconstructed[cur+i] = reconstructed[cur + i]
            cur += size
            #print(size)

        elif com[0] == 'COPY1': # 6
            size = com[1]
            it = com[2]
            # print(cur -it)
            for i in range(size):
                # if cur -it == 9:
                #     print(reconstructed[cur -it + i])
                reconstructed[cur+i] = reconstructed[cur -it + i]
            cur += size
            #print(size)

        elif com[0] == 'COPY2': # 7
            size = com[1]
            it = com[2]
            for i in range(size):
                reconstructed[cur+i] = reconstructed[cur + it + i]
            cur += size
            lm+=1
            #print(size)

        elif com[0] == 'COPY':
            type_ = com[1]
            init = com[2]
            size = com[3]

            if type_ == 0:
                for i in range(size):
                    reconstructed[cur+i] = reconstructed[init + i]
            elif type_ == 1:
                for i in range(size):
                    reconstructed[cur+i] = reconstructed[init - i]
            elif type_ == 2:
                for i in range(size):
                    reconstructed[cur+i] = reconstructed[init + i]
            elif type_ == 3:
                for i in range(size):
                    reconstructed[cur+i] = reconstructed[init - i]
            
            cur += size
            #print(size)

    return bytes(reconstructed)[:lenNew]


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

    LL = 9
    MM = 9 + valid
    i = MM +singletons

    coms = 0
    comms = []
    while(i < len(DELTA)):

        pico = (DELTA[LL]  >> coms) & 1

        if (pico == 1):
            c = ['ADD', 1, [DELTA[MM]]]
            comms.append(c)
            MM+=1
        
        coms += 1

        if(coms == 8):
            LL += 1
            coms = 0

        first_two = int.from_bytes(DELTA[i:i+2], byteorder='little', signed=False)
        opcode = first_two & 7
        length = first_two >> 3

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

    reconstructed = interpreter(comms, OLD, size_of_update)
    print(reconstructed == NEW)