package p4_aas.Submodels.NetworkInfrastructure.YamlParser;

import java.util.List;
import java.util.stream.Collectors;
import org.yaml.snakeyaml.Yaml;

import p4_aas.StaticProperties;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

@SuppressWarnings("unchecked")
public class YamlParser {
    public List<Switches> parseYaml() {
        List<Switches> switches = new LinkedList<Switches>();
        try {
            Map<String, List<Map<String, Object>>> data = (Map<String, List<Map<String, Object>>>) 
                new Yaml().load(new BufferedReader(new InputStreamReader(this.getClass().getClassLoader().getResourceAsStream(StaticProperties.YAML_FILE_PATH))));

            for (Map.Entry<String, List<Map<String, Object>>> entry : data.entrySet()) {
                Map<String, List<Rules>> rulesMap = new HashMap<>();

                Switches sw = new Switches(entry.getKey());
                entry.getValue().forEach(el -> {
                    rulesMap.put(
                        el.get("program").toString(), 
                        Rules.formatRules(el.get("rules")));
                });

                sw.setRules(rulesMap);
                switches.add(sw);
            }
            System.out.println(switches.stream().filter(el -> el.getID().equals("s1")).map(el -> el.getRules("standard")).collect(Collectors.toList()));

        } catch (Exception e) {
            e.printStackTrace();
        }

        return switches;
    }
}
