"""
This software will communicate, on the serial bus, with the controller that
is measuring temperature. On a request, this software will notify you about
the current temperaute measured.
"""

import serial
import smtplib
import imaplib
import time
from email.mime.text import MIMEText
from email.parser import BytesParser, Parser
from email.policy import default
import email

def init_serial():
    SERIAL_PORT = "/dev/ttyS3"
    SERIAL_RATE = 9600
    return serial.Serial(SERIAL_PORT, SERIAL_RATE)

def init_mail():
    s = smtplib.SMTP("smtp.gmail.com", 587)
    s.ehlo()
    s.starttls()

    s.login("communication mail", "password for this mail")

    return s

def ask_for_info(serial_var):
    time.sleep(2)
    serial_var.write(b'AT+TER')

def toggle_led(serial_var):
    time.sleep(2)
    serial_var.write(b'AT+LED')

def read_temperature(serial_var):
    reading = serial_var.readline().decode("ASCII")
    print(reading[0])
    temperature = 5/1024*100*ord(reading[0])
    serial_var.flushInput()
    return int(temperature)

def send_email(server, value):
    msg = MIMEText('Temperature is: %s degrees of Celsius' % str(value))
    msg['Subject'] = 'Temperature'
    msg['From'] = 'communication mail'
    msg['To'] = 'your mail'
    print(msg)
    server.sendmail(msg['From'], msg['To'], msg.as_string())
    print('Mail sent.')

def init_check_inbox():
    mail = imaplib.IMAP4_SSL('imap.gmail.com')
    mail.login('communication mail', 'password for this mail')
    return mail

def check_inbox(mail):
    mail.list()
    # Out: list of "folders" aka labels in gmail.
    mail.select("inbox") # connect to inbox.

    result, data = mail.search(None, '(FROM "your mail")')

    ids = data[0] # data is a list.
    id_list = ids.split() # ids is a space separated string
    if len(id_list) == 0:
        return -1
    latest_email_id = id_list[-1] # get the latest

    result, data = mail.fetch(latest_email_id, "(RFC822)") # fetch the email body (RFC822) for the given ID

    raw_email = data[0][1] # here's the body, which is raw text of the whole email
    # including headers and alternate payloads
    return raw_email.decode('utf-8')

def mail_parse(raw_mail):
    email_message = email.message_from_string(raw_mail)
    for part in email_message.walk():
        if part.get_content_type() == "text/plain":
            body = part.get_payload(decode=True)
        else:
            continue
    return body

def delete_mails(mail):
    mail.select("inbox")
    typ, data = mail.search(None, 'ALL')
    for num in data[0].split():
        mail.store(num, '+FLAGS', r'(\Deleted)')
    mail.expunge()

def main():

    server = init_mail()
    mail = init_check_inbox()
    serial = init_serial()

    temperature = 0

    t0 = time.time()

    while 1:

        raw_mail = check_inbox(mail)
        t1 = time.time()
        if (t1 - t0) > 500:
            server = init_mail()
            mail = init_check_inbox()
            t0 = time.time()
        if raw_mail == -1:
            continue

        mail_body = mail_parse(raw_mail)
        print(mail_body[0:4].decode())

        if mail_body[0:4].decode() == 'Info':
            ask_for_info(serial)
            time.sleep(1)
            temperature = read_temperature(serial)
            print(temperature)
            send_email(server, temperature)
            user_in = '0'
            temperature = 0
            delete_mails(mail)
            continue




if __name__ == "__main__":
    main()
