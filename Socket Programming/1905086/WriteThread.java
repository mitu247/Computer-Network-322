import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.Time;
import java.util.Objects;
import java.util.Scanner;
import java.util.Timer;

public class WriteThread implements Runnable {

private Thread thr;
private Utility networkUtil;
private String clientDirectory = "client/";
String name;

public WriteThread(Utility networkUtil, String name) {
    this.networkUtil = networkUtil;
    this.name = name;
    this.thr = new Thread(this);
    thr.start();
}
public void fileUpload(String filename) throws IOException, ClassNotFoundException {
    Object recOb= networkUtil.read(); // chunk size got from the server
    if (recOb instanceof Message) {
        if (((Message) recOb).getText().equalsIgnoreCase("File too large")) {
            System.out.println("The file is too large to send");
            return;
        }

        int chunkSize = ((Message) recOb).getChunkSize();
        int fileId = ((Message) recOb).getFileId();
        System.out.println("Server => ChunkSize: " + chunkSize + ", File ID: " + fileId);

        FileInputStream fis = new FileInputStream(clientDirectory + filename);
        byte[] aChunk;
        int initSize = 0, check;
        ByteFile byteFile = new ByteFile();
        while (true) {
            aChunk = new byte[chunkSize];
            check = fis.read(aChunk, 0, chunkSize); // reading chunk by chunk

            if (check == -1) {
                networkUtil.write("completed");
                Object com;
                com = networkUtil.read();
                System.out.println((String)com);

                fis.close();
                break;
            }

            byteFile.setChunk(aChunk);
            byteFile.setChunkSize(check);
            networkUtil.write(byteFile);

            try {
                recOb = networkUtil.read();
            } catch (SocketTimeoutException e) {
                System.out.println("Server ack timeout");
                byteFile.setNotification("acktimeout");
                networkUtil.write(byteFile);
            }
            if (((String) recOb).equalsIgnoreCase("ack")) {
                System.out.print("... "); // File is being uploaded
            }
            initSize += check;
//            System.out.println("File sent: " + initSize);
        }
        System.out.println();
    }
}
public void run() {
    try {
        Scanner input = new Scanner(System.in);
        while (true) {
            System.out.println("Select your option: ");
            System.out.println("1.See all users");
            System.out.println("2.Previous uploaded files & Download");
            System.out.println("3.Public Files & Download");
            System.out.println("4.File request");
            System.out.println("5.Unread messages");
            System.out.println("6.Upload a file");
            System.out.println("7.Logout");
            System.out.print("Option: ");
            String option = input.nextLine();

            if(option.equalsIgnoreCase("1")) {
                Message message = new Message();
                message.setFrom(this.name);
                message.setText(option);
                networkUtil.write(message);
                Object o = networkUtil.read();
                System.out.println("Users---------");
                if (o instanceof Message) {
                    Message obj = (Message) o;
                    System.out.println(obj.getText());
                    if (obj.getClients() != null) {
                        String[] clients = obj.getClients();
                        for (int i = 0; i < clients.length; i++) {
                            System.out.println(i + 1 + ". " + clients[i]);
                        }
                    }
                }
                System.out.println();
            }
            else if(option.equalsIgnoreCase("2")){
                Message message = new Message();
                message.setFrom(this.name);
                message.setText(option);
                String file_name = "";
                networkUtil.write(message);
                while(true){
                    Object obj = networkUtil.read();
                    if(!(obj instanceof Message)){
                        break;
                    }
                    Message msg = (Message)obj;
                    System.out.println(msg.getFileId()+" [] "+msg.getFileName()+" [] "+msg.getPrivacy());
                    file_name = msg.getFileName();
                }


                System.out.println("Enter the file ID you want to download (Type \"NO\" to cancel): ");
                String fileID = input.nextLine();
                networkUtil.write(fileID);
                if (fileID.equalsIgnoreCase("no")) continue;

                FileOutputStream fos = new FileOutputStream("download/"+file_name);
                Object byteFile;
                while (true) {
                    byteFile = networkUtil.read();
                    if(!(byteFile instanceof ByteFile)) {
                        System.out.println((String)byteFile);
                        break;
                    }
                    fos.write(((ByteFile)byteFile).getChunk(), 0, ((ByteFile)byteFile).getChunkSize());
                }
            }
            else if(option.equalsIgnoreCase("3")){
                Message message = new Message();
                message.setFrom(this.name);
                message.setText(option);
                networkUtil.write(message);
                String file_name = "";
                while(true){
                    Object obj = networkUtil.read();
                    if(!(obj instanceof Message)){
                        break;
                    }
                    Message msg = (Message)obj;
                    System.out.println(msg.getFrom()+" [] "+msg.getFileName()+" [] "+msg.getFileId());
                    file_name = msg.getFileName();
                }
                System.out.println("Enter the file ID you want to download (type \"NO\" to cancel): ");
                String fileID = input.nextLine();
                networkUtil.write(fileID);
                if (fileID.equalsIgnoreCase("no")) continue;

                FileOutputStream fos = new FileOutputStream("download/"+file_name);
                Object byteFile;
                while (true) {
                    byteFile = networkUtil.read();
                    if(!(byteFile instanceof ByteFile)) {
                        System.out.println((String)byteFile);
                        break;
                    }
                    fos.write(((ByteFile)byteFile).getChunk(), 0, ((ByteFile)byteFile).getChunkSize());
                }
            }
            else if(option.equalsIgnoreCase("4")) {
                Message message = new Message();
                message.setFrom(this.name);
                message.setText(option);
                networkUtil.write(message);

                String fname;
                System.out.println("Please provide a brief description of file (you must mention the filename with extension)");
                fname = input.nextLine();
                message.setText(fname);
                networkUtil.write(message);
            }
            else if(option.equalsIgnoreCase("5")) {
                Message message = new Message();
                message.setFrom(this.name);
                message.setText(option);
                networkUtil.write(message);

                Object messages = networkUtil.read();

                if (((StoredMessages) messages).inboxLength() == 0) {
                    ((StoredMessages) messages).printAllMessages(); // ack after someone uploads my requested file
                    networkUtil.write("nomsg");
                    continue;
                }
                networkUtil.write("hasmsg");

                if (messages instanceof StoredMessages) {
                    System.out.println("Your messages ---- ");
                    ((StoredMessages) messages).printAllMessages();
                    System.out.println();
                }

                System.out.println("Would you like to upload any of the files requested? (yes / no)");
                option = input.nextLine();
                if (option.equalsIgnoreCase("no")) {
                    networkUtil.write("not_uploading");
                    continue;
                }
                networkUtil.write("uploading");

                File file = new File(clientDirectory);
                File[] files = file.listFiles();
                System.out.println("Files for uploading: ");

                for(int i = 0; i < files.length; i++){
                    System.out.println("File Name: "+files[i].getName()+" File Size: "+files[i].length());
                }
                System.out.println("Please mention the request id:");
                int reqID = Integer.parseInt(input.nextLine());
                System.out.println("Please provide file name: ");
                String filename = input.nextLine();
                System.out.println("Please provide file size: ");
                String fileSize = input.nextLine();
                System.out.println("Filesize: " + fileSize);
                String privacy = "public";

                Message msg = new Message();
                msg.setFrom(this.name);
                msg.setFileName(filename);
                msg.setFileSize(fileSize);
                msg.setReqId(reqID);
                msg.setText("6"); // file upload
                msg.setPrivacy(privacy);
                networkUtil.write(msg);
                fileUpload(filename);
            }
            else if(option.equalsIgnoreCase("6")){
               File file = new File(clientDirectory);
               File[] files = file.listFiles();
               System.out.println("Show files for uploading: ");
               for(int i = 0; i<files.length; i++){
                   System.out.println("File Name: "+files[i].getName()+" File Size: "+files[i].length());
               }
               System.out.println("Please provide file name: ");
               String filename = input.nextLine();
               System.out.println("Please provide file size: ");
               String fileSize = input.nextLine();
               System.out.println("Please Provide File Privacy(public/private): ");
               String privacy = input.nextLine();
               Message msg = new Message();
               msg.setFrom(this.name);
               msg.setFileName(filename);
               msg.setFileSize(fileSize);
               msg.setText(option);
               msg.setPrivacy(privacy);
               networkUtil.write(msg);
               fileUpload(filename);
            }
            else if(option.equalsIgnoreCase("7")) {
                networkUtil.closeConnection();
                break;
            }
        }
    } catch (Exception e) {
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
