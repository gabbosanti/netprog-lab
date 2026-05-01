package p4_aas.NetworkController.Serialization;

import java.util.List;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;

@JsonIgnoreProperties(ignoreUnknown = true)
public class RuleDescribers {
    @JsonProperty("TableName")
    private String tableName;

    @JsonProperty("TableId")
    private String tableId;

    @JsonProperty("Keys")
    private List<Key> keys;

    @JsonProperty("ActionName")
    private String actionName;

    @JsonProperty("ActionId")
    private String actionId;

    @JsonProperty("ActionParams")
    private List<ActionParam> actionParams;

    // Getters and setters
    public String getTableName() {
        return tableName;
    }

    public void setTableName(String TableName) {
        this.tableName = TableName;
    }

    public String getTableId() {
        return tableId;
    }

    public void setTableId(String TableId) {
        this.tableId = TableId;
    }

    public List<Key> getKeys() {
        return keys;
    }

    public void setKeys(List<Key> Keys) {
        this.keys = Keys;
    }

    public String getActionName() {
        return actionName;
    }

    public void setActionName(String ActionName) {
        this.actionName = ActionName;
    }

    public String getActionId() {
        return actionId;
    }

    public void setActionId(String ActionId) {
        this.actionId = ActionId;
    }

    public List<ActionParam> getActionParams() {
        return actionParams;
    }

    public void setActionParams(List<ActionParam> ActionParams) {
        this.actionParams = ActionParams;
    }

    public String getUrlForRule() {
        return "idTable=" + this.getTableId() + "&idAction=" + this.getActionId();
    }

    @Override
    public String toString() {
        return "SwitchDescribers [tableName=" + tableName + ", tableId=" + tableId + ", keys=" + keys + ", actionName="
                + actionName + ", actionId=" + actionId + ", actionParams=" + actionParams + "]";
    }
}