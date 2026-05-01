package p4_aas.Submodels.NetworkInfrastructure.YamlParser;

import java.util.List;
import java.util.Map;

public class Switches {
    private String ID;
    private Map<String, List<Rules>> rules;

    public Switches(String ID) {
        this.ID = ID;
    }

    public String getID() {
        return this.ID;
    }

    public void setRules(Map<String, List<Rules>> rules) {
        this.rules = rules;
    }

    public List<Rules> getRules(String program) {
        return this.rules.get(program);
    }

    @Override
    public String toString() {
        return "S1{" +
                "ID='" + ID + '\'' +
                ", rules=" + rules +
                '}';
    }
}
