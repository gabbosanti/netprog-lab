package p4_aas.Submodels.NetworkInfrastructure.YamlParser;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;

@SuppressWarnings({ "unchecked", "rawtypes" })
public class Rules {
    private String table;
    private List<Keys> keys;
    private String action;
    private List<String> actionParam;

    static List<Rules> formatRules(Object obj) {
        ArrayList<Object> jsonString = (ArrayList) obj;

        ObjectMapper mapper = new ObjectMapper();
        mapper.configure(JsonParser.Feature.ALLOW_UNQUOTED_FIELD_NAMES, true);
        List<Rules> rulesList = new LinkedList<>();
        
        try {
            jsonString.forEach(el -> {
                try {
                    Rules rule = mapper.readValue(new ObjectMapper().writeValueAsString(el) , new TypeReference<Rules>(){});
                    rulesList.add(rule);
                } catch (IOException e) {
                    e.printStackTrace();
                }

            });
        } catch (Exception e) {
            e.printStackTrace();
        }
        return rulesList;
    }

    public String getTable() {
        return table;
    }

    public void setTable(String table) {
        this.table = table;
    }

    public String getKeys() {
        return keys.stream().findFirst().get().getValue();
    }

    public void setKeyValue(String newValue) {
        this.keys.stream().filter(el -> el.getValue().equals("X.X.X.X")).forEach(el -> el.setValue(newValue));
    }

    public void setKeys(List<Keys> keys) {
        this.keys = keys;
    }

    public String getAction() {
        return action;
    }

    public void setAction(String action) {
        this.action = action;
    }

    public List<String> getActionParam() {
        return actionParam;
    }

    //mac_address, port
    public String getValueForMatchedActionParam(String pattern) {
        switch(pattern) {
            case "mac_address": 
                return this.getActionParam().stream().filter(el -> el.length() > 4).findFirst().get();
            case "port": 
                return this.getActionParam().stream().filter(el -> el.length() == 1).findFirst().get();
            default: 
                return null;
        }
    }

    public void setActionParam(List<String> actionParam) {
        this.actionParam = actionParam;
    }

    @Override
    public String toString() {
        return "Rules{" +
                "table='" + table + '\'' +
                ", keys=" + keys +
                ", action='" + action + '\'' +
                ", actionParam=" + actionParam +
                '}';
    }
}
