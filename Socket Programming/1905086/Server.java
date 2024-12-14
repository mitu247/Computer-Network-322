import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class Server {

    private ServerSocket serverSocket;
    public HashMap<Integer,String> fileMap;
    public HashMap<String, Utility> clientMap;
    private List<StoredMessages> storedMessages;

    Server() {
        clientMap = new HashMap<>();
        fileMap = new HashMap<>();
        storedMessages = new ArrayList<>();
        try {
            serverSocket = new ServerSocket(33333);
            while (true) {
                Socket clientSocket = serverSocket.accept();
                serve(clientSocket);
            }
        } catch (Exception e) {
            System.out.println("Server starts:" + e);
        }
    }

    public void serve(Socket clientSocket) throws IOException, ClassNotFoundException {
        Utility networkUtil = new Utility(clientSocket);
        String clientName = (String) networkUtil.read();

        String serverDirectory = "server/";
        if(clientMap.containsKey(clientName)){
            Message message = new Message();
            message.setText("Already connected");
            networkUtil.write(message);
            networkUtil.closeConnection();
        }
        else {
            this.clientMap.put(clientName, networkUtil);
            File file1 = new File(serverDirectory+clientName);
            File file2 = new File(serverDirectory+clientName+"/public");
            File file3 = new File(serverDirectory+clientName+"/private");
            file1.mkdir();
            file2.mkdir();
            file3.mkdir();
            Message message = new Message();
            message.setText("Successful");
            StoredMessages sm = new StoredMessages();
            sm.setMyName(clientName);
            storedMessages.add(sm);
            networkUtil.write(message);

        }
        new ReadThreadServer(clientName, clientMap,fileMap,storedMessages,networkUtil,serverDirectory);
        //new WriteThreadServer(clientMap,clientName);
    }

    public static void main(String args[]) {
        Server server = new Server();
    }
}
