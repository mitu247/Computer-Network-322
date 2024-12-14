import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

public class StoredMessages implements Serializable {
    private String myName;
    private List<String> messages;
    private List<Integer> allRequestIds;
    private List<Integer> myRequestIds;

    public StoredMessages() {
        messages = new ArrayList<>();
        allRequestIds = new ArrayList<>();
        myRequestIds = new ArrayList<>();
    }

    public String getMyName() {
        return myName;
    }

    public void setMyName(String myName) {
        this.myName = myName;
    }

    public void addMessages(String message) {
        this.messages.add(message);
    }

    public void addRequestId(int requestId) {
        this.allRequestIds.add(requestId);
    }

    public void printAllMessages() {
        for (int i=0; i<messages.size(); i++) {
            if (allRequestIds.size()!=0)
                System.out.println((i+1) + ". " + messages.get(i) + ", request ID: " + allRequestIds.get(i));
            else
                System.out.println(messages.get(i));
        }

        messages.clear();
        allRequestIds.clear();
    }

    public void addToMyReqIds(int reqId) {
        this.myRequestIds.add(reqId);
    }

    public List<Integer> getMyRequestIds() {
        return this.myRequestIds;
    }

    public int inboxLength() {
        return this.allRequestIds.size();
    }
}
