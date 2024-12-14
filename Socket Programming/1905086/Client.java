import java.util.Scanner;

public class Client {

    public Client(String serverAddress, int serverPort) {
        try {
            System.out.print("Enter name of the client: ");
            Scanner scanner = new Scanner(System.in);
            String clientName = scanner.nextLine();
            Utility networkUtil = new Utility(serverAddress, serverPort);
            networkUtil.write(clientName);
            Object obj = networkUtil.read();
            if(((Message)obj).getText().equalsIgnoreCase("already connected")){
                System.out.println(clientName+" you are already connected");
            }
            else if(((Message)obj).getText().equalsIgnoreCase("successful")){
                new WriteThread(networkUtil, clientName);
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public static void main(String args[]) {
        String serverAddress = "127.0.0.1";
        int serverPort = 33333;
        Client client = new Client(serverAddress, serverPort);
    }
}
