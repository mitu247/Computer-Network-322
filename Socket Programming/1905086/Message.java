import java.io.Serializable;

public class Message implements Serializable {
    private String from;
    private String to;
    private String text;
    private int fileId;
    private int reqId;
    private String fileSize;
    private String fileName;
    private String privacy;
    private String[] clients;
    private String[] clientFiles;
    private int chunkSize;
    public Message() {
        this.from = "";
        this.to = "";
        this.text = "";
        this.fileSize = "";
    }
    public String[] getClientFiles() {
        return clientFiles;
    }
    public void setClientFiles(String[] clientFiles) {
        this.clientFiles = clientFiles;
    }
    public String getFrom() {
        return from;
    }

    public void setFrom(String from) {
        this.from = from;
    }

    public String getTo() {
        return to;
    }

    public void setTo(String to) {
        this.to = to;
    }

    public String getText() {
        return text;
    }
    public void setClients(String[] clients){
        this.clients = clients;
    }
    public String[] getClients(){
        return clients;
    }

    public String getFileSize() {
        return fileSize;
    }

    public void setFileSize(String fileSize) {
        this.fileSize = fileSize;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public String getPrivacy() {
        return privacy;
    }

    public void setPrivacy(String privacy) {
        this.privacy = privacy;
    }

    public int getFileId() {
        return fileId;
    }

    public int getChunkSize() {
        return chunkSize;
    }

    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }

    public void setFileId(int fileId) {
        this.fileId = fileId;
    }

    public void setText(String text) {
        this.text = text;
    }

    public int getReqId() {
        return reqId;
    }

    public void setReqId(int reqId) {
        this.reqId = reqId;
    }
}