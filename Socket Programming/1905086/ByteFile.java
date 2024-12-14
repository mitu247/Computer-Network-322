import java.io.Serializable;

public class ByteFile implements Serializable {
    private byte[] chunk;
    private String notification;
    private int chunkSize;
    public ByteFile() {
        this.notification = "null";
    }

    public byte[] getChunk() {
        return chunk;
    }

    public void setChunk(byte[] chunk) {
        this.chunk = chunk;
    }

    public String getNotification() {
        return notification;
    }

    public void setNotification(String notification) {
        this.notification = notification;
    }

    public int getChunkSize() {
        return chunkSize;
    }

    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }
}
