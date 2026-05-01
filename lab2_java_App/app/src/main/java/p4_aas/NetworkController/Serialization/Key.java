package p4_aas.NetworkController.Serialization;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;

@JsonIgnoreProperties(ignoreUnknown = true)
public class Key {
    @JsonProperty("Name")
    private String name;

    @JsonProperty("Bitwidth")
    private int bitwidth;

    @JsonProperty("MatchType")
    private String matchType;

    @JsonProperty("Pattern")
    private String pattern;

    // Getters and Setters
    public String getName() {
        return name;
    }
    public void setName(String Name) {
        this.name = Name;
    }
    public int getBitwidth() {
        return bitwidth;
    }
    public void setBitwidth(int Bitwidth) {
        this.bitwidth = Bitwidth;
    }
    public String getMatchType() {
        return matchType;
    }
    public void setMatchType(String MatchType) {
        this.matchType = MatchType;
    }
    public String getPattern() {
        return pattern;
    }
    public void setPattern(String Pattern) {
        this.pattern = Pattern;
    }
    @Override
    public String toString() {
        return "Key [name=" + name + ", bitwidth=" + bitwidth + ", matchType=" + matchType + ", pattern=" + pattern
                + "]";
    }

    // Getters and setters
}

