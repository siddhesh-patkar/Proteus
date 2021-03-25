from twython import Twython
from twython import TwythonStreamer
import smtplib
from email.mime.image import MIMEImage
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
import time
from datetime import datetime
import urllib.request
import sys

class streamer(TwythonStreamer):
   def on_success(self,data):
      print(data)

class Twitter:

   def setup(self, CustomerKey, CustomerSecret, Token, SecretToken):
      self.twitter = Twython(CustomerKey,CustomerSecret,Token,SecretToken)
      
   def tweet(self, message):
      self.twitter.update_status(status=message)
   
   def tweetImage(self, Message, Image):
      photo = open(Image, 'rb')
      response = self.twitter.upload_media(media=photo)
      self.twitter.update_status(status=Message, media_ids=[response['media_id']])
   
   def tweetVideo(self, Message, Vid):
      video = open(Vid, 'rb')
      response = self.twitter.upload_video(media=video, media_type='video/mp4', check_progress=True)
      self.twitter.update_status(status=Message, media_ids=[response['media_id']])
      
   def searchFor(self,Question):
      data = self.twitter.search(q=Question, count=1)
      if data['statuses'] != []:
         Message = data['statuses'][0]['text']
         UserName = data['statuses'][0]['user']['screen_name']
         Date = data['statuses'][0]['created_at'][4:]
         datetime_object = datetime.strptime(Date, '%b %d %H:%M:%S %z %Y')
      else:
         Message = ""
         UserName = ""
         datetime_object = datetime.utcfromtimestamp(0)
      return(Message,UserName,datetime_object)
      
   def searchImage(self,Question,ImageName):
      data = self.twitter.search(q=Question, count=1)
      if data['statuses'] != []:
         Message = data['statuses'][0]['text']
         UserName = data['statuses'][0]['user']['screen_name']
         Date = data['statuses'][0]['created_at'][4:]
         datetime_object = datetime.strptime(Date, '%b %d %H:%M:%S %z %Y')
         if 'media' in data['statuses'][0]['entities']:
            ImageReceived = True
            fileType = data['statuses'][0]['entities']['media'][0]['media_url'].split('.')[-1]
            PictureName = ImageName+fileType
            file = open(PictureName,'wb')
            file.write(urllib.request.urlopen(data['statuses'][0]['entities']['media'][0]['media_url']).read())
            file.close()
         else:
            PictureName = ""
      else:
         PictureName = ""
         Message = ""
         UserName = ""
         datetime_object = datetime.utcfromtimestamp(0)
         
      return(Message,UserName,datetime_object,PictureName)
class Email:
   attachmentNames = []
   recipientEmails = ""
   def emailLogIn(self, email, password, server):
      self.emailServer = server
      self.userEmail = email
      self.userPassword = password
      
   def emailStructure(self,Subject,EmailContent,*RecipientEmails):
      #Turns list of Emails into string
      for i in range(len(RecipientEmails)):
         self.recipientEmails += RecipientEmails[i] + ','
      #removes last commer
      self.recipientEmails = self.recipientEmails[:-1]
      self.emailSubject = Subject
      self.emailContent = EmailContent
      
   def emailAttachment(self,*file):
      for i in range(len(file)):
         self.attachmentNames.append(file[i])
      
   def sendEmail(self):
      email = MIMEMultipart()
      email['Subject'] = self.emailSubject
      email['From'] = self.userEmail
      email['To'] = self.recipientEmails

      email.attach(MIMEText(self.emailContent, 'plain'))

      if len(self.attachmentNames) != 0:
         for i in range(len(self.attachmentNames)):
            filename = self.attachmentNames[i]
            attachment = open(filename, "rb")
            
            part = MIMEBase('application', 'octet-stream')
            part.set_payload((attachment).read())
            encoders.encode_base64(part)
            part.add_header('Content-Disposition', "attachment; filename= %s" % filename)
 
            email.attach(part)
      try:
         server = smtplib.SMTP_SSL(self.emailServer)
         server.ehlo()
         server.login(self.userEmail, self.userPassword)
         server.send_message(email)
         server.quit()
         print("Email sent")
      
      except Exception as e:
         print("Email Failed, Check login", file=sys.stderr)
         print(e, file=sys.stderr)
      
class MQTT:
   pass