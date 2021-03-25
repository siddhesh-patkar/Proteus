/*
 * FTP Serveur for ESP8266
 * based on FTP Serveur for Arduino Due and Ethernet shield (W5100) or WIZ820io (W5200)
 * based on Jean-Michel Gallego's work
 * modified to work with esp8266 SPIFFS by David Paiva david@nailbuster.com
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FtpServer.h"
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined ESP32
#include <WiFi.h>
#include "SPIFFS.h"
#endif
#include <WiFiClient.h>
#include <FS.h>


FtpServer::FtpServer ()
                : ftpServer( FTP_CTRL_PORT ), dataServer( FTP_DATA_PORT_PASV ) { progressHandler=NULL; }

void FtpServer::begin(String uname, String pword)
{
  // Tells the ftp server to begin listening for incoming connection
	_FTP_USER=uname;
	_FTP_PASS = pword;

	ftpServer.begin();
	delay(10);
	dataServer.begin();	
	delay(10);
	millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
	millisDelay = 0;
	cmdStatus = 0;
   iniVariables();    
}

void FtpServer::iniVariables()
{
  // Default for data port
  dataPort = FTP_DATA_PORT_PASV;
  
  // Default Data connection is Active
  dataPassiveConn = true;
  
  // Set the root directory
  strcpy( cwdName, "/" );

  rnfrCmd = false;
  transferStatus = 0;
  
}

bool FtpServer::handleFTP()
{
  if((int32_t) ( millisDelay - millis() ) > 0 )
    return client.connected();

  if (ftpServer.hasClient()) {
	  client.stop();
	  client = ftpServer.available();
  }
  
  if( cmdStatus == 0 )
  {
    if( client.connected())
      disconnectClient();
    cmdStatus = 1;
  }
  else if( cmdStatus == 1 )         // Ftp server waiting for connection
  {
    abortTransfer();
    iniVariables();
    #ifdef FTP_DEBUG
	Serial.println("Ftp server waiting for connection on port "+ String(FTP_CTRL_PORT));
    #endif
    cmdStatus = 2;
  }
  else if( cmdStatus == 2 )         // Ftp server idle
  {
   		
    if( client.connected() )                // A client connected
    {
      clientConnected();      
      millisEndConnection = millis() + 10 * 1000 ; // wait client id during 10 s.
      cmdStatus = 3;
    }
  }
  else if( readChar() > 0 )         // got response
  {
    if( cmdStatus == 3 )            // Ftp server waiting for user identity
      if( userIdentity() )
        cmdStatus = 4;
      else
        cmdStatus = 0;
    else if( cmdStatus == 4 )       // Ftp server waiting for user registration
      if( userPassword() )
      {
        cmdStatus = 5;
        millisEndConnection = millis() + millisTimeOut;
      }
      else
        cmdStatus = 0;
    else if( cmdStatus == 5 )       // Ftp server waiting for user command
      if( ! processCommand())
        cmdStatus = 0;
      else
        millisEndConnection = millis() + millisTimeOut;
  }
  else if (!client.connected() || !client)
  {
	  cmdStatus = 1;
      #ifdef FTP_DEBUG
	    Serial.println("client disconnected");
	  #endif
  }

  if( transferStatus == 1 )         // Retrieve data
  {
    if( ! doRetrieve())
      transferStatus = 0;
    else if (progressHandler != NULL)
      progressHandler();   
  }
  else if( transferStatus == 2 )    // Store data
  {
    if( ! doStore())
      transferStatus = 0;
    else if (progressHandler != NULL)
      progressHandler();  
  }
  else if( cmdStatus > 2 && ! ((int32_t) ( millisEndConnection - millis() ) > 0 ))
  {
	  client.print("530 Timeout\r\n");
    millisDelay = millis() + 200;    // delay of 200 ms
    cmdStatus = 0;
  }

  return client.connected();
}

void FtpServer::clientConnected()
{
  #ifdef FTP_DEBUG
	Serial.println("Client connected!");
  #endif
  client.print( "220--- Welcome to FTP for ESP8266/ESP32 ---\r\n");
  client.print( "220---   By David Paiva   ---\r\n");
  client.print( "220 --   Version "+ String(FTP_SERVER_VERSION) +"   --\r\n");
  iCL = 0;
}

void FtpServer::disconnectClient()
{
  #ifdef FTP_DEBUG
	Serial.println(" Disconnecting client");
  #endif
  abortTransfer();
  client.print("221 Goodbye\r\n");
  client.stop();
}

boolean FtpServer::userIdentity()
{	
  if( strcmp( command, "USER" ))
    client.print( "500 Syntax error\r\n");
  if( strcmp( parameters, _FTP_USER.c_str() ))
    client.print( "530 user not found\r\n");
  else
  {
    client.print( "331 OK. Password required\r\n");
    strcpy( cwdName, "/" );
    return true;
  }
  millisDelay = millis() + 100;  // delay of 100 ms
  return false;
}

boolean FtpServer::userPassword()
{
  if( strcmp( command, "PASS" ))
    client.print( "500 Syntax error\r\n");
  else if( strcmp( parameters, _FTP_PASS.c_str() ))
    client.print( "530 \r\n");
  else
  {
    #ifdef FTP_DEBUG
      Serial.println( "OK. Waiting for commands.");
    #endif
    client.print( "230 OK.\r\n");
    return true;
  }
  millisDelay = millis() + 100;  // delay of 100 ms
  return false;
}

boolean FtpServer::processCommand()
{
  ///////////////////////////////////////
  //                                   //
  //      ACCESS CONTROL COMMANDS      //
  //                                   //
  ///////////////////////////////////////

  //
  //  CDUP - Change to Parent Directory 
  //
  if( ! strcmp( command, "CDUP" ))
  {
	  client.print("250 Ok. Current directory is " + String(cwdName) + "\r\n");
  }
  //
  //  CWD - Change Working Directory
  //
  else if( ! strcmp( command, "CWD" ))
  {
    char path[ FTP_CWD_SIZE ];
    if( strcmp( parameters, "." ) == 0 )  // 'CWD .' is the same as PWD command
      client.print( "257 \"" + String(cwdName) + "\" is current directory.\r\n");
    else 
      {       
        client.print( "250 Ok. Current directory is " + String(cwdName) + "\r\n" );
      }
    
  }
  //
  //  PWD - Print Directory
  //
  else if( ! strcmp( command, "PWD" ))
    client.print( "257 \"" + String(cwdName) + "\" is current directory.\r\n");
  //
  //  QUIT
  //
  else if( ! strcmp( command, "QUIT" ))
  {
    disconnectClient();
    return false;
  }

  ///////////////////////////////////////
  //                                   //
  //    TRANSFER PARAMETER COMMANDS    //
  //                                   //
  ///////////////////////////////////////

  //
  //  MODE - Transfer Mode 
  //
  else if( ! strcmp( command, "MODE" ))
  {
    if( ! strcmp( parameters, "S" ))
      client.print( "200 S Ok\r\n");
    // else if( ! strcmp( parameters, "B" ))
    //  client.print( "200 B Ok\r\n";
    else
      client.print( "504 Only S(tream) is suported\r\n");
  }
  //
  //  PASV - Passive Connection management
  //
  else if( ! strcmp( command, "PASV" ))
  { if (data.connected()) 
         data.stop();
     dataIp = client.localIP();	
	  dataPort = FTP_DATA_PORT_PASV;
    //data.connect( dataIp, dataPort );
    //data = dataServer.available();
     #ifdef FTP_DEBUG
	     Serial.println("Connection management set to passive");
        Serial.println( "Data port set to " + String(dataPort));
      #endif
      client.print( "227 Entering Passive Mode ("+ String(dataIp[0]) + "," + String(dataIp[1])+","+ String(dataIp[2])+","+ String(dataIp[3])+","+String( dataPort >> 8 ) +","+String ( dataPort & 255 )+").\r\n");   
      dataPassiveConn = true;
  
  }
  //
  //  PORT - Data Port
  //
  else if( ! strcmp( command, "PORT" ))
  {
	if (data) data.stop();
    // get IP of data client
    dataIp[ 0 ] = atoi( parameters );
    char * p = strchr( parameters, ',' );
    for( uint8_t i = 1; i < 4; i ++ )
    {
      dataIp[ i ] = atoi( ++ p );
      p = strchr( p, ',' );
    }
    // get port of data client
    dataPort = 256 * atoi( ++ p );
    p = strchr( p, ',' );
    dataPort += atoi( ++ p );
    if( p == NULL )
      client.print( "501 Can't interpret parameters\r\n");
    else
    {
      
		client.print("200 PORT command successful\r\n");
      dataPassiveConn = false;
    }
  }
  //
  //  STRU - File Structure
  //
  else if( ! strcmp( command, "STRU" ))
  {
    if( ! strcmp( parameters, "F" ))
      client.print( "200 F Ok\r\n");
    // else if( ! strcmp( parameters, "R" ))
    //  client.print( "200 B Ok\r\n";
    else
      client.print( "504 Only F(ile) is suported\r\n");
  }
  //
  //  TYPE - Data Type
  //
  else if( ! strcmp( command, "TYPE" ))
  {
    if( ! strcmp( parameters, "A" ))
      client.print( "200 TYPE is now ASCII\r\n");
    else if( ! strcmp( parameters, "I" ))
      client.print( "200 TYPE is now 8-bit binary\r\n");
    else
      client.print( "504 Unknow TYPE\r\n");
  }

  ///////////////////////////////////////
  //                                   //
  //        FTP SERVICE COMMANDS       //
  //                                   //
  ///////////////////////////////////////

  //
  //  ABOR - Abort
  //
  else if( ! strcmp( command, "ABOR" ))
  {
    abortTransfer();
    client.print( "226 Data connection closed\r\n");
  }
  //
  //  DELE - Delete a File 
  //
  else if( ! strcmp( command, "DELE" ))
  {
    char path[ FTP_CWD_SIZE ];
    if( strlen( parameters ) == 0 )
      client.print( "501 No file name\r\n");
    else if( makePath( path ))
    {
      if( ! SPIFFS.exists( path ))
        client.print( "550 File " + String(parameters) + " not found\r\n");
      else
      {
        if( SPIFFS.remove( path ))
          client.print( "250 Deleted " + String(parameters)+"\r\n" );
        else
          client.print( "450 Can't delete " + String(parameters)+"\r\n");
      }
    }
  }
  //
  //  LIST - List 
  //
  else if( ! strcmp( command, "LIST" ))
  {
    if( ! dataConnect())
      client.print( "425 No data connection\r\n");
    else
    {
      client.print( "150 Accepted data connection\r\n");
      uint16_t nm = 0;
#ifdef ESP8266
      Dir dir=SPIFFS.openDir(cwdName);
     // if( !SPIFFS.exists(cwdName))
     //   client.print( "550 Can't open directory " + String(cwdName) + "\r\n" );
     // else
      {
        while( dir.next())
        {
			String fn, fs;
			fn = dir.fileName();
			fn.remove(0, 1);
			fs = String(dir.fileSize());
         char buf[128];
			sprintf(buf, "%s   1 %-10s %-10s %10lu Jan  1  1980 %s\r\n", "rwx", "user", "user", dir.fileSize(), fn.c_str());
			data.print(buf);
			Serial.print(buf);
         nm ++;
        }
        client.print( "226 " + String(nm) + " matches total\r\n");
      }
#elif defined ESP32
			File root = SPIFFS.open(cwdName);
			if(!root){
					client.print( "550 Can't open directory " + String(cwdName) + "\r\n");
					// return;
			} else {
				// if(!root.isDirectory()){
				// 		Serial.println("Not a directory");
				// 		return;
				// }

				File file = root.openNextFile();
				while(file){
					if(file.isDirectory()){
						data.println( "+r,s <DIR> " + String(file.name()));
						// Serial.print("  DIR : ");
						// Serial.println(file.name());
						// if(levels){
						// 	listDir(fs, file.name(), levels -1);
						// }
					} else {
						String fn, fs;
						fn = file.name();
						// fn.remove(0, 1);
						fs = String(file.size());
						data.println( "+r,s" + fs);
						data.println( ",\t" + fn );
						nm ++;
					}
					file = root.openNextFile();
				}
				client.print( "226 " + String(nm) + " matches total\r\n");
			}
#endif
      data.stop();
    }
  }
  //
  //  MLSD - Listing for Machine Processing (see RFC 3659)
  //
  else if( ! strcmp( command, "MLSD" ))
  {
    if( ! dataConnect())
      client.print( "425 No data connection MLSD\r\n");
    else
    {
	  client.print( "150 Accepted data connection\r\n");
      uint16_t nm = 0;
#ifdef ESP8266
      Dir dir= SPIFFS.openDir(cwdName);
      char dtStr[ 15 ];
    //  if(!SPIFFS.exists(cwdName))
    //    client.print( "550 Can't open directory " +String(parameters)+"\r\n" );
    //  else
      {
        while( dir.next())
		{
			String fn,fs;
			fn = dir.fileName();
			fn.remove(0, 1);
			fs = String(dir.fileSize());
          data.print( "Type=file;Size=" + fs + ";"+"modify=20000101160656;" +" " + fn + "\r\n");
          nm ++;
        }
        client.print( "226-options: -a -l\r\n");
        client.print( "226 " + String(nm) + " matches total\r\n");
      }
#elif defined ESP32
			File root = SPIFFS.open(cwdName);
			// if(!root){
			// 		client.println( "550 Can't open directory " + String(cwdName) );
			// 		// return;
			// } else {
				// if(!root.isDirectory()){
				// 		Serial.println("Not a directory");
				// 		return;
				// }

				File file = root.openNextFile();
				while(file){
					// if(file.isDirectory()){
					// 	data.println( "+r,s <DIR> " + String(file.name()));
					// 	// Serial.print("  DIR : ");
					// 	// Serial.println(file.name());
					// 	// if(levels){
					// 	// 	listDir(fs, file.name(), levels -1);
					// 	// }
					// } else {
						String fn, fs;
						fn = file.name();
						fn.remove(0, 1);
						fs = String(file.size());
						data.println( "Type=file;Size=" + fs + ";"+"modify=20000101160656;" +" " + fn);
						nm ++;
					// }
					file = root.openNextFile();
				}
				client.println( "226-options: -a -l");
				client.println( "226 " + String(nm) + " matches total");
			// }
#endif
      data.stop();
    }
  }
  //
  //  NLST - Name List 
  //
  else if( ! strcmp( command, "NLST" ))
  {
    if( ! dataConnect())
      client.print( "425 No data connection\r\n");
    else
    {
      client.print( "150 Accepted data connection\r\n");
      uint16_t nm = 0;
#ifdef ESP8266
      Dir dir=SPIFFS.openDir(cwdName);
     // if( !SPIFFS.exists( cwdName ))
     //   client.println( "550 Can't open directory " + String(parameters));
     // else
      {
        while( dir.next())
        {
          data.print( dir.fileName()+"\r\n");
          nm ++;
        }
        client.print( "226 " + String(nm) + " matches total\r\n");
      }
#elif defined ESP32
		File root = SPIFFS.open(cwdName);
		if(!root){
				client.println( "550 Can't open directory " + String(cwdName) );
		} else {

			File file = root.openNextFile();
			while(file){
				data.println( file.name());
					nm ++;
				file = root.openNextFile();
			}
			client.println( "226 " + String(nm) + " matches total");
		}
#endif
      data.stop();
    }
  }
  //
  //  NOOP
  //
  else if( ! strcmp( command, "NOOP" ))
  {
    // dataPort = 0;
    client.print( "200 Zzz...\r\n");
  }
  //
  //  RETR - Retrieve
  //
  else if( ! strcmp( command, "RETR" ))
  {
    char path[ FTP_CWD_SIZE ];
    if( strlen( parameters ) == 0 )
      client.println( "501 No file name");
    else if( makePath( path ))
	{
		file = SPIFFS.open(path, "r");
      if( !file)
        client.print( "550 File " +String(parameters)+ " not found\r\n");
      else if( !file )
        client.print( "450 Can't open " +String(parameters) + "r\n");
      else if( ! dataConnect())
        client.print( "425 No data connection\r\n");
      else
      {
        #ifdef FTP_DEBUG
		  Serial.println("Sending " + String(parameters));
        #endif
        client.print( "150-Connected to port "+ String(dataPort) + "\r\n");
        client.print( "150 " + String(file.size()) + " bytes to download\r\n");
        millisBeginTrans = millis();
        bytesTransfered = 0;
        transferStatus = 1;
      }
    }
  }
  //
  //  STOR - Store
  //
  else if( ! strcmp( command, "STOR" ))
  {
    char path[ FTP_CWD_SIZE ];
    if( strlen( parameters ) == 0 )
      client.print( "501 No file name\r\n");
    else if( makePath( path ))
    {
		file = SPIFFS.open(path, "w");
      if( !file)
        client.print( "451 Can't open/create " +String(parameters) + "\r\n");
      else if( ! dataConnect())
      {
        client.print( "425 No data connection\r\n");
        file.close();
      }
      else
      {
        #ifdef FTP_DEBUG
          Serial.println( "Receiving " +String(parameters));
        #endif
        client.print( "150 Connected to port " + String(dataPort) + "\r\n");
        millisBeginTrans = millis();
        bytesTransfered = 0;
        transferStatus = 2;
      }
    }
  }
  //
  //  MKD - Make Directory
  //
  else if( ! strcmp( command, "MKD" ))
  {
	  client.print( "550 Can't create \"" + String(parameters) + "\r\n");  //not support on espyet
  }
  //
  //  RMD - Remove a Directory 
  //
  else if( ! strcmp( command, "RMD" ))
  {
	  client.print( "501 Can't delete \"" +String(parameters) + "\r\n");
	
  }
  //
  //  RNFR - Rename From 
  //
  else if( ! strcmp( command, "RNFR" ))
  {
    buf[ 0 ] = 0;
    if( strlen( parameters ) == 0 )
      client.print( "501 No file name\r\n");
    else if( makePath( buf ))
    {
      if( ! SPIFFS.exists( buf ))
        client.print( "550 File " +String(parameters)+ " not found\r\n");
      else
      {
        #ifdef FTP_DEBUG
		  Serial.println("Renaming " + String(buf));
        #endif
        client.print( "350 RNFR accepted - file exists, ready for destination\r\n");     
        rnfrCmd = true;
      }
    }
  }
  //
  //  RNTO - Rename To 
  //
  else if( ! strcmp( command, "RNTO" ))
  {  
    char path[ FTP_CWD_SIZE ];
    char dir[ FTP_FIL_SIZE ];
    if( strlen( buf ) == 0 || ! rnfrCmd )
      client.print( "503 Need RNFR before RNTO\r\n");
    else if( strlen( parameters ) == 0 )
      client.print( "501 No file name\r\n");
    else if( makePath( path ))
    {
      if( SPIFFS.exists( path ))
        client.print( "553 " +String(parameters)+ " already exists\r\n");
      else
      {          
            #ifdef FTP_DEBUG
		  Serial.println("Renaming " + String(buf) + " to " + String(path));
            #endif
            if( SPIFFS.rename( buf, path ))
              client.print( "250 File successfully renamed or moved\r\n");
            else
				client.print( "451 Rename/move failure\r\n");
                                 
      }
    }
    else {
       Serial.print("Unknown command:");
       Serial.println(command);
    }  
    rnfrCmd = false;
  }

  ///////////////////////////////////////
  //                                   //
  //   EXTENSIONS COMMANDS (RFC 3659)  //
  //                                   //
  ///////////////////////////////////////

  //
  //  FEAT - New Features
  //
  else if( ! strcmp( command, "FEAT" ))
  {
    client.print( "211-Extensions suported:\r\n");
    client.print( " MLSD\r\n");
    client.print( " SIZE\r\n");
    client.print( " MDTM\r\n");
    client.print( "211 END\r\n");
  }
  //
  //  MDTM - File Modification Time (see RFC 3659)
  //
  else if (!strcmp(command, "MDTM"))
  {
	  client.print("550 Unable to retrieve time\r\n");
  }

  //
  //  SIZE - Size of the file
  //
  else if( ! strcmp( command, "SIZE" ))
  {
    char path[ FTP_CWD_SIZE ];
    if( strlen( parameters ) == 0 )
      client.print( "501 No file name\r\n");
    else if( makePath( path ))
	{
		file = SPIFFS.open(path, "r");
      if(!file)
         client.print( "450 Can't open " +String(parameters) + "\r\n");
      else
      {
        client.print( "213 " + String(file.size())+ "\r\n");
        file.close();
      }
    }
  }
  //
  //  SITE - System command
  //
  else if( ! strcmp( command, "SITE" ))
  {
      client.print( "500 Unknown SITE command " +String(parameters) + "\r\n");
  }
  //
  //  Unrecognized commands ...
  //
  else
    client.print( "500 Unknown command\r\n");
  
  return true;
}

boolean FtpServer::dataConnect()
{
  unsigned long startTime = millis();
  //wait 5 seconds for a data connection
  if (!data.connected())
  {
	  while (!dataServer.hasClient() && millis() - startTime < 10000)
	  {
		  //delay(100);
		  yield();
	  }
	  if (dataServer.hasClient()) {
		  data.stop();
		  data = dataServer.available();
			#ifdef FTP_DEBUG
		      Serial.println("ftpdataserver client....");
			#endif
		
	  }
  }
 
  return data.connected();

}

boolean FtpServer::doRetrieve()
{
if (data.connected())
{
	int16_t nb = file.readBytes(buf, FTP_BUF_SIZE);
	if (nb > 0)
  	{
		data.write((uint8_t*)buf, nb);
		bytesTransfered += nb;
		return true;
	}
}
closeTransfer();
return false;
}

boolean FtpServer::doStore()
{
  if( data.connected() )
  {
		// Avoid blocking by never reading more bytes than are available
    int navail = data.available();
    if (navail <= 0) return true;
    // And be sure not to overflow buf.
    if (navail > FTP_BUF_SIZE) navail = FTP_BUF_SIZE;
    int16_t nb = data.read((uint8_t*) buf, navail );
    // int16_t nb = data.readBytes((uint8_t*) buf, FTP_BUF_SIZE );
    if( nb > 0 )
    {
      // Serial.println( millis() << " " << nb << endl;
      file.write((uint8_t*) buf, nb );
      bytesTransfered += nb;
    }
    return true;
  }
  closeTransfer();
  return false;
}

void FtpServer::closeTransfer()
{
  uint32_t deltaT = (int32_t) ( millis() - millisBeginTrans );
  if( deltaT > 0 && bytesTransfered > 0 )
  {
    client.print( "226-File successfully transferred\r\n");
    client.print( "226 " + String(deltaT) + " ms, "+ String(bytesTransfered / deltaT) + " kbytes/s\r\n");
  }
  else
    client.print( "226 File successfully transferred\r\n");
  
  file.close();
  data.stop();
}

void FtpServer::abortTransfer()
{
  if( transferStatus > 0 )
  {
    file.close();
    data.stop(); 
    client.print( "426 Transfer aborted\r\n"  );
    #ifdef FTP_DEBUG
      Serial.println( "Transfer aborted!") ;
    #endif
  }
  transferStatus = 0;
}

// Read a char from client connected to ftp server
//
//  update cmdLine and command buffers, iCL and parameters pointers
//
//  return:
//    -2 if buffer cmdLine is full
//    -1 if line not completed
//     0 if empty line received
//    length of cmdLine (positive) if no empty line received 

int8_t FtpServer::readChar()
{
  int8_t rc = -1;

  if( client.available())
  {
    char c = client.read();
	 // char c;
	 // client.readBytes((uint8_t*) c, 1);
    #ifdef FTP_DEBUG
      Serial.print( c);
    #endif
    if( c == '\\' )
      c = '/';
    if( c != '\r' )
      if( c != '\n' )
      {
        if( iCL < FTP_CMD_SIZE )
          cmdLine[ iCL ++ ] = c;
        else
          rc = -2; //  Line too long
      }
      else
      {
        cmdLine[ iCL ] = 0;
        command[ 0 ] = 0;
        parameters = NULL;
        // empty line?
        if( iCL == 0 )
          rc = 0;
        else
        {
          rc = iCL;
          // search for space between command and parameters
          parameters = strchr( cmdLine, ' ' );
          if( parameters != NULL )
          {
            if( parameters - cmdLine > 4 )
              rc = -2; // Syntax error
            else
            {
              strncpy( command, cmdLine, parameters - cmdLine );
              command[ parameters - cmdLine ] = 0;
              
              while( * ( ++ parameters ) == ' ' )
                ;
            }
          }
          else if( strlen( cmdLine ) > 4 )
            rc = -2; // Syntax error.
          else
            strcpy( command, cmdLine );
          iCL = 0;
        }
      }
    if( rc > 0 )
      for( uint8_t i = 0 ; i < strlen( command ); i ++ )
        command[ i ] = toupper( command[ i ] );
    if( rc == -2 )
    {
      iCL = 0;
      client.println( "500 Syntax error");
    }
  }
  return rc;
}

// Make complete path/name from cwdName and parameters
//
// 3 possible cases: parameters can be absolute path, relative path or only the name
//
// parameters:
//   fullName : where to store the path/name
//
// return:
//    true, if done

boolean FtpServer::makePath( char * fullName )
{
  return makePath( fullName, parameters );
}

boolean FtpServer::makePath( char * fullName, char * param )
{
  if( param == NULL )
    param = parameters;
    
  // Root or empty?
  if( strcmp( param, "/" ) == 0 || strlen( param ) == 0 )
  {
    strcpy( fullName, "/" );
    return true;
  }
  // If relative path, concatenate with current dir
  if( param[0] != '/' ) 
  {
    strcpy( fullName, cwdName );
    if( fullName[ strlen( fullName ) - 1 ] != '/' )
      strncat( fullName, "/", FTP_CWD_SIZE );
    strncat( fullName, param, FTP_CWD_SIZE );
  }
  else
    strcpy( fullName, param );
  // If ends with '/', remove it
  uint16_t strl = strlen( fullName ) - 1;
  if( fullName[ strl ] == '/' && strl > 1 )
    fullName[ strl ] = 0;
  if( strlen( fullName ) < FTP_CWD_SIZE )
    return true;

  client.println( "500 Command line too long");
  return false;
}

// Calculate year, month, day, hour, minute and second
//   from first parameter sent by MDTM command (YYYYMMDDHHMMSS)
//
// parameters:
//   pyear, pmonth, pday, phour, pminute and psecond: pointer of
//     variables where to store data
//
// return:
//    0 if parameter is not YYYYMMDDHHMMSS
//    length of parameter + space

uint8_t FtpServer::getDateTime( uint16_t * pyear, uint8_t * pmonth, uint8_t * pday,
                                uint8_t * phour, uint8_t * pminute, uint8_t * psecond )
{
  char dt[ 15 ];

  // Date/time are expressed as a 14 digits long string
  //   terminated by a space and followed by name of file
  if( strlen( parameters ) < 15 || parameters[ 14 ] != ' ' )
    return 0;
  for( uint8_t i = 0; i < 14; i++ )
    if( ! isdigit( parameters[ i ]))
      return 0;

  strncpy( dt, parameters, 14 );
  dt[ 14 ] = 0;
  * psecond = atoi( dt + 12 ); 
  dt[ 12 ] = 0;
  * pminute = atoi( dt + 10 );
  dt[ 10 ] = 0;
  * phour = atoi( dt + 8 );
  dt[ 8 ] = 0;
  * pday = atoi( dt + 6 );
  dt[ 6 ] = 0 ;
  * pmonth = atoi( dt + 4 );
  dt[ 4 ] = 0 ;
  * pyear = atoi( dt );
  return 15;
}

// Create string YYYYMMDDHHMMSS from date and time
//
// parameters:
//    date, time 
//    tstr: where to store the string. Must be at least 15 characters long
//
// return:
//    pointer to tstr

char * FtpServer::makeDateTimeStr( char * tstr, uint16_t date, uint16_t time )
{
  sprintf( tstr, "%04u%02u%02u%02u%02u%02u",
           (( date & 0xFE00 ) >> 9 ) + 1980, ( date & 0x01E0 ) >> 5, date & 0x001F,
           ( time & 0xF800 ) >> 11, ( time & 0x07E0 ) >> 5, ( time & 0x001F ) << 1 );            
  return tstr;
}

