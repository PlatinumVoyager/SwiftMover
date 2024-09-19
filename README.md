![image](https://github.com/user-attachments/assets/ac9a75bf-451d-4796-abb6-99c386800661)


# SwiftMover
Source code for a deprecated "LOTL file transfer technique" which would utilize the Windows BITS subsystem

* What is LOTL (Living Off The Land) is a generalized technique in which Cyber operators utilize pre-existing tools/infrastructure upon a targeted host to reduce to likelyhood that any footprints/metadata of the initial operation took place.
* In particular *SwiftMover* would have used the Windows 10/11 "Background Intelligent Transfer Service (BITS)" in C to upload/download remote files.

![image](https://github.com/user-attachments/assets/c4eefed1-e302-43ae-9055-55157dc6646b)

## The nice thing about BITS is that it cares for the end user.
* Developing a separate custom subsystem for taking account into user bandwidth in order to not arouse suspicion is not needed.
* Creating additional lower-level file transfer/access methods is no longer needed.
* Appearing as a legitimate Windows internal service is no longer needed.
* Clandestine Process Injection techniques deployed to local processes to move data to/from the internal network is no longer needed.
  
![image](https://github.com/user-attachments/assets/701eed91-c135-4024-bfa7-fa10cbf0b55f)
