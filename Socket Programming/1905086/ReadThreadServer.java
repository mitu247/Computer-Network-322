import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.SocketException;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
public class ReadThreadServer implements Runnable {
private Thread thr;
private Utility networkUtil;
private String serverDirectory;
private final int MAX_BUFFER_SIZE = 10000;
private final int MIN_CHUNK_SIZE = 10;
private final int MAX_CHUNK_SIZE = 100;
private int usedSpace = 0;
private String clientName;
public HashMap<String, Utility> clientMap;
public HashMap<Integer,String> fileMap;
public List<StoredMessages> listOfStoredMessages;

public ReadThreadServer(String clientName, HashMap<String, Utility> map,HashMap<Integer,String>f_map,List<StoredMessages> sm,Utility networkUtil,String serverDirectory) {
    this.clientName = clientName;
    this.clientMap = map;
    this.fileMap = f_map;
    this.listOfStoredMessages = sm;
    this.networkUtil = networkUtil;
    this.serverDirectory = serverDirectory;
    this.thr = new Thread(this);
    thr.start();
}
public void receiveFile(String from, Message obj) throws IOException, ClassNotFoundException {
    String fileName = obj.getFileName();
    int fileSize = Integer.parseInt(obj.getFileSize());
    String filePrivacy = obj.getPrivacy();

    if ((MAX_BUFFER_SIZE-usedSpace) < fileSize) {
        Message m = new Message();
        m.setText("File too large");
        networkUtil.write(m);
        return;
    }

    int chunkSize = (new Random().nextInt(MAX_CHUNK_SIZE-MIN_CHUNK_SIZE)) + MIN_CHUNK_SIZE;
    Message msg = new Message();
    msg.setFileId(IdGen.fileId);
    msg.setChunkSize(chunkSize);
    networkUtil.write(msg);
    IdGen.fileId++;

    FileOutputStream fos = new FileOutputStream(this.serverDirectory+from+"/"+filePrivacy+"/"+fileName);
    Object byteFile;
    int preUsedSpace = usedSpace;

    while (true) {
        byteFile = networkUtil.read();
        if (!(byteFile instanceof ByteFile)) {
            System.out.println((String)byteFile); // completed
            String tx;
            System.out.println(usedSpace-preUsedSpace+" "+fileSize);
            if((usedSpace-preUsedSpace) == fileSize){
                tx = "Successful";
                fileMap.put(msg.getFileId(),this.serverDirectory+from+"/"+filePrivacy+"/"+fileName);
                fos.close();
            }
            else{
                tx = "error occurred";
                usedSpace = preUsedSpace;
                fos.close();
                new File(this.serverDirectory+from+"/"+filePrivacy+"/"+fileName).delete();
            }
            networkUtil.write(tx);
//            networkUtil.write("successful");
            return;
        }
        if (((ByteFile)byteFile).getNotification().equalsIgnoreCase("acktimeout")) {
            fos.close();
            new File(this.serverDirectory+from+"/"+filePrivacy+"/"+fileName).delete(); // delete the file
            usedSpace = preUsedSpace;
            return;
        }

        fos.write(((ByteFile) byteFile).getChunk(), 0, ((ByteFile) byteFile).getChunkSize());
        //System.out.println("Chunk got: " + ((ByteFile) byteFile).getChunkSize());

        networkUtil.write("ack"); // ack after getting a chunk from client
        usedSpace += ((ByteFile) byteFile).getChunkSize();
    }

    // file received successfully
}

public void run() {
    try {
        while (true) {
            Object o = networkUtil.read();
            if (o instanceof Message) {
                Message obj = (Message) o;
                String from = obj.getFrom();
                String text = obj.getText();
                if(text.equalsIgnoreCase("1")){
                    File file = new File(serverDirectory);
                    String [] allUser = file.list();
                    for(int i = 0; i< allUser.length; i++){
                       if(clientMap.containsKey(allUser[i])){
                           allUser[i] = allUser[i]+" [] Active Now";
                       }
                       else{
                           allUser[i] = allUser[i]+ " [] Inactive";
                       }
                    }
                    Message msg = new Message();
                    msg.setClients(allUser);
                    //Utility networkUtil = clientMap.get(from);
                    networkUtil.write(msg);
                }
                else if(text.equalsIgnoreCase("2")){
                    File file1 = new File(serverDirectory+from+"/public");
                    String [] publicFiles = file1.list();
                    if(publicFiles != null){
                        for(int i = 0; i< publicFiles.length; i++){
                            Message msg = new Message();
                            for (int id: fileMap.keySet())
                            if (fileMap.get(id).equalsIgnoreCase(serverDirectory+from+"/public/"+publicFiles[i]))
                            msg.setFileId(id);
                            msg.setFileName(publicFiles[i]);
                            msg.setPrivacy("public");
                            networkUtil.write(msg);
                        }
                    }
                    File file2 = new File(serverDirectory+from+"/private");
                    String [] privateFiles = file2.list();
                    if(privateFiles != null){
                        for(int i = 0; i< privateFiles.length; i++){
                            Message msg = new Message();
                            for (int id: fileMap.keySet())
                            if (fileMap.get(id).equalsIgnoreCase(serverDirectory+from+"/private/"+privateFiles[i]))
                            msg.setFileId(id);
                            msg.setFileName(privateFiles[i]);
                            msg.setPrivacy("private");
                            networkUtil.write(msg);
                        }
                    }
                    networkUtil.write("completed");
                    Object object = networkUtil.read();
                    String fileID = (String)object;
                    if (fileID.equalsIgnoreCase("no")) continue;

                    int id = Integer.parseInt(fileID);
                    String path = fileMap.get(id);
                    if(path!=null){
                        FileInputStream fis = new FileInputStream(path);
                        //System.out.println(path);
                        byte[] aChunk = new byte[MAX_CHUNK_SIZE];
                        int check;
                        ByteFile byteFile = new ByteFile();
                        while (true) {
                            check = fis.read(aChunk, 0, MAX_CHUNK_SIZE); // reading chunk by chunk
                            if(check==-1){
                                networkUtil.write("Download Complete");
                                break;
                            }
                            byteFile.setChunk(aChunk);
                            byteFile.setChunkSize(check);
                            networkUtil.write(byteFile);
                    }
                    }
                }
                else if(text.equalsIgnoreCase("3")){
                    File file = new File(serverDirectory);
                    File[] clients = file.listFiles();
                    for(int i = 0; i< clients.length; i++){
                        File file1 = new File(serverDirectory+clients[i].getName()+"/public/");
                        //System.out.println(clients[i].getName());
                        String[] publicFiles = file1.list();
                        //System.out.println(publicFiles.length);
                        for(int j=0; j<publicFiles.length; j++){
                            Message message = new Message();
                            message.setFrom(clients[i].getName());
                            for (int id: fileMap.keySet())
                            if (fileMap.get(id).equalsIgnoreCase(serverDirectory+clients[i].getName()+"/public/"+publicFiles[j]))
                            {
                               // System.out.println(id);
                                /*System.out.println(publicFiles[j]);*/
                                message.setFileId(id);
                            }
                            message.setFileName(publicFiles[j]);
                            networkUtil.write(message);
                        }
                    }
                    networkUtil.write("completed");
                    Object object = networkUtil.read();
                    String fileID = (String)object;
                    if (fileID.equalsIgnoreCase("no")) continue;

                    int id = Integer.parseInt(fileID);
                    String path = fileMap.get(id);
                    if(path!=null){
                        FileInputStream fis = new FileInputStream(path);
                        //System.out.println(path);
                        byte[] aChunk = new byte[MAX_CHUNK_SIZE];
                        int check;
                        ByteFile byteFile = new ByteFile();
                        while (true) {
                            check = fis.read(aChunk, 0, MAX_CHUNK_SIZE); // reading chunk by chunk
                            if(check==-1){
                                networkUtil.write("Download Complete");
                                break;
                            }
                            byteFile.setChunk(aChunk);
                            byteFile.setChunkSize(check);
                            networkUtil.write(byteFile);
                        }
                    }
                }
                else if(text.equalsIgnoreCase("4")){
                    Object message = networkUtil.read();

                    if (message instanceof Message) {
                        String reqFileName = ((Message) message).getText();
                        for (int i = 0; i < listOfStoredMessages.size(); i++) {
                            if (listOfStoredMessages.get(i).getMyName().equalsIgnoreCase(from)) {
                                listOfStoredMessages.get(i).addToMyReqIds(IdGen.reqId);
                            }
                            else {
                                listOfStoredMessages.get(i).addMessages(from + " requested for file \"" + reqFileName + "\"");
                                listOfStoredMessages.get(i).addRequestId(IdGen.reqId);
                            }
                        }

                        IdGen.reqId++;
                    }
                }
                else if(text.equalsIgnoreCase("5")){
                    System.out.println("Show messages to: " + from);
                    for (int i = 0; i< listOfStoredMessages.size(); i++) {
                        if (listOfStoredMessages.get(i).getMyName().equalsIgnoreCase(from)) {
                            StoredMessages sm = listOfStoredMessages.get(i);
                            networkUtil.write(sm);
                            break;
                        }
                    }
                    String feedback = (String) networkUtil.read();
                    if (feedback.equalsIgnoreCase("nomsg"))
                        continue;

                    feedback = (String) networkUtil.read();
                    if (feedback.equalsIgnoreCase("not_uploading")) continue;

                    int requestID = -1;
                    Object newObj = networkUtil.read();
                    if (newObj instanceof Message) {
                        requestID = ((Message) newObj).getReqId();
                        receiveFile(from, (Message) newObj);
                    }


                    for (int i=0; i<listOfStoredMessages.size(); i++) {
                        for (int reqid: listOfStoredMessages.get(i).getMyRequestIds()) {
                            if (reqid == requestID) {
                                listOfStoredMessages.get(i).addMessages("The file has been uploaded by \"" + from + "\"");
                            }
                        }
                    }
                }
                else if(text.equalsIgnoreCase("6")) {
                    receiveFile(from,obj);
                }
                }
        }
    } catch (Exception e) {
        if (e instanceof IOException) {
            this.clientMap.remove(this.clientName);
            System.out.println("[" + clientName + "] logged out of the system");
        }
        System.out.println(e);
    } finally {
        try {
            networkUtil.closeConnection();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
}

