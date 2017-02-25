#!/usr/bin/env python2
from pocketsphinx import *
import pyaudio
import sys
import os

modeldir = '/usr/local/share/pocketsphinx/model/en-us'
hmm = '/usr/local/share/pocketsphinx/model/en-us/en-us'
dic = '4292.dic'
lm= '4292.lm'

config = Decoder.default_config()
config.set_string('-hmm', os.path.join(modeldir, 'en-us'))
#config.set_string('-lm', lm)
config.set_string('-dict', dic)
config.set_string('-logfn', '/dev/null')
config.set_string('-kws', 'keywords.txt')
config.set_float('-kws_threshold', 1e+10)

decoder = Decoder(config)

p = pyaudio.PyAudio()

stream = p.open(format=pyaudio.paInt16, channels=1, rate=16000, input=True, frames_per_buffer=1024)
stream.start_stream()
in_speech_bf = True
decoder.start_utt()
os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 82')
while True:
    buf = stream.read(1024)
    if buf:
        decoder.process_raw(buf, False, False)
        try:
            if  decoder.hyp().hypstr != '':
                print 'Partial decoding result:', decoder.hyp().hypstr
        except AttributeError:
            pass
        if decoder.get_in_speech():
            sys.stdout.write('.')
            sys.stdout.flush()
        if decoder.get_in_speech() != in_speech_bf:
            in_speech_bf = decoder.get_in_speech()
            if not in_speech_bf:
                decoder.end_utt()
                try:
                    if  decoder.hyp().hypstr != '':
			result = decoder.hyp().hypstr
			if result == 'LIGHTS RED':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 81')
			elif result == 'LIGHTS GREEN':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 82')
			elif result == 'LIGHTS BLUE':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 83')
			elif result == 'LIGHTS YELLOW':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 84')
			elif result == 'LIGHTS CYAN':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 85')
			elif result == 'LIGHTS PURPLE':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 86')
			elif result == 'LIGHTS WHITE':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 87')
			elif result == 'LIGHTS OFF':
				os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 88')
                        print 'Stream decoding result:', decoder.hyp().hypstr
                except AttributeError:
                    pass
                decoder.start_utt()
    else:
        break
decoder.end_utt()
os.system('cd /home/pi/RF24RaspberryCommunicator; sudo ./remote -m 81')
print 'An Error occured:', decoder.hyp().hypstr
