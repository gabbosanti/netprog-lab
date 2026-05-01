package p4_aas.NetworkController.Serialization;

public class JsonObject {
    public String Table;
    public Keys[] Keys;
    public String Action;
    public String[] ActionParam;

    public static class Keys {
        public String Value;
    }
}
