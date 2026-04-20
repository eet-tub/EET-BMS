import serial
import struct




def bms_get_cell_data_as_string(dev:serial,bmsnum):
    
    dev.reset_input_buffer() #Reset the input buffer in case something was left uninterpreted
    data = struct.pack("4B",bmsnum,2,0,0)
    dev.write(data)
    data = dev.read(3)
    header = struct.unpack_from("3B",data)
    data = dev.read(int(header[2]))

    payloadlenformat = str(len(data)-1) + "s" + "B" #Build Payload Format String plus one Byte for checksum
    payload = struct.unpack_from(payloadlenformat,data,offset=0)
    checksum = payload[1] #Check if checksum is valid here

    return payload[0].decode('ascii')

    

