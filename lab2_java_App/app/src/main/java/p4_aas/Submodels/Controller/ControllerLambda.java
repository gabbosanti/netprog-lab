package p4_aas.Submodels.Controller;

import java.util.Map;
import java.util.LinkedList;
import java.util.List;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.math.*;

import org.eclipse.basyx.submodel.metamodel.api.qualifier.haskind.ModelingKind;
import org.eclipse.basyx.submodel.metamodel.map.Submodel;
import org.eclipse.basyx.submodel.metamodel.map.qualifier.LangStrings;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.SubmodelElement;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.Property;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.valuetype.ValueType;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.operation.Operation;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.operation.OperationVariable;

import p4_aas.NetworkController.NetworkController;
import p4_aas.NetworkController.Serialization.RuleDescribers;
import p4_aas.NetworkController.Utils.ApiEnum;
import p4_aas.Submodels.Utils.Utils;

import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.IOException;

public class ControllerLambda {
    private NetworkController client;
    private Utils utils;

    public ControllerLambda(NetworkController client, Utils utils) {
        this.client = client;
        this.utils = utils;
    }

    public Function<Map<String, SubmodelElement>, SubmodelElement[]> getRules(String url) {
        return (args) -> {
            List<SubmodelElement> finalRes = new LinkedList<>();
            List<String> actualRules = client.getRules(url);

            actualRules.forEach(el -> {
                finalRes.add(this.createProperty("Rule_" + actualRules.indexOf(el), el));
            });

            return finalRes.toArray(new SubmodelElement[finalRes.size()]);
        };
    }

    private SubmodelElement createProperty(String idShort, Object value) {
        SubmodelElement el = new Property(idShort, value);
        el.setKind(ModelingKind.TEMPLATE);
        return el;
    }

    public Function<Map<String, SubmodelElement>, SubmodelElement[]> deleteRules(String url) {
        return (args) -> {
            client.deleteRule(url + (BigInteger) args.get("ruleID").getValue());
            return new SubmodelElement[]{};
        };
    }

    /**
     * Initilizing createRules submodel with new Operations, 
     * after getting current (and updated) rule Describers from the P4 Program.
     * @param controllerId to switch this submodel refers
     * @param sub Submodel to be updated
     */
    public void getRuleDescribers(int controllerId, Submodel sub) {
        client.getRuleDescribers(
            controllerId == 1 ? ApiEnum.RULEDESCRIBER_SW1.url : ApiEnum.RULEDESCRIBER_SW2.url)
            .forEach(ruleDescriber -> {
                sub.addSubmodelElement(this.createNewRuleOperation(ruleDescriber, controllerId));
        });
    }

    /**
     * 
     * @param ruleDescriber
     * @param controllerId to which the created rule will refer to.
     * @return nee Operation for given ruleDescriber, 
     * with all correct input fields and descriptions
     */
    private Operation createNewRuleOperation(RuleDescribers ruleDescriber, int controllerId) {
        List<OperationVariable> inputVars = new LinkedList<>();

        ruleDescriber.getKeys().forEach(k -> {
            inputVars.addAll(utils.getCustomInputVariables(Map.of(k.getName() + ":" + k.getMatchType(), ValueType.String)));
        });

        ruleDescriber.getActionParams().forEach(ac -> {
            inputVars.addAll(utils.getCustomInputVariables(Map.of(ac.getName() + ":" + ac.getPattern(), ValueType.String)));
        });

        Operation op = new Operation(ruleDescriber.getTableName() + ":" + ruleDescriber.getActionName());
        op.setDescription(new LangStrings("English", "idTable=" + ruleDescriber.getTableId() + "&idAction=" + ruleDescriber.getActionId()));
        op.setInputVariables(inputVars);
        op.setWrappedInvokable(this.putRuleOnController(op, controllerId));

        return op;
    }

    private Function<Map<String, SubmodelElement>, SubmodelElement[]> putRuleOnController(Operation op, int controllerId) {
        return (args) -> {
            // 1. Start Timer Totale
            long startTotal = System.nanoTime();

            Map<String, String> inputValues = op
                .getInputVariables()
                .stream()
                .collect(Collectors.toMap(
                    (el) -> el.getValue().getIdShort(), 
                    (el) -> (String) args.get(el.getValue().getIdShort()).getValue()
                ));

            String URL = (controllerId == 1 ? 
                ApiEnum.ADDRULE_SW1.url + op.getDescription().get("English") : 
                ApiEnum.ADDRULE_SW2.url + op.getDescription().get("English"));

            // 2. Start Timer Rete
            long startNet = System.nanoTime();

            client.postRule(URL, inputValues);
            
            // 3. Stop Timer Rete
            long endNet = System.nanoTime();
            
            // 4. Stop Timer Totale
            long endTotal = System.nanoTime();

            // Calcoli
            double netTime = (endNet - startNet) / 1_000_000.0;
            double totalTime = (endTotal - startTotal) / 1_000_000.0;

            // 5. SCRITTURA SU FILE (Chiama il metodo helper qui sotto)
            logPerformanceToFile(op.getIdShort(), totalTime, netTime);

            return new SubmodelElement[]{};
        };
    }

    // AGGIUNGI QUESTO METODO HELPER ALLA CLASSE
    private synchronized void logPerformanceToFile(String opName, double totalTime, double netTime) {
        String fileName = "aas_metrics.csv"; // Il file verrà creato nella root del progetto Java
        double overhead = totalTime - netTime;
        
        try (FileWriter fw = new FileWriter(fileName, true); // true = append mode
             PrintWriter pw = new PrintWriter(fw)) {
            
            // Formato: Operazione, TempoTotale, TempoRete, Overhead
            pw.printf("%s,%.3f,%.3f,%.3f%n", opName, totalTime, netTime, overhead);
            
        } catch (IOException e) {
            System.err.println("Errore scrittura metriche: " + e.getMessage());
        }
    }
    
    public Function<Map<String, SubmodelElement>, SubmodelElement[]> refreshRules(int controllerId, Submodel createRules) {
        return (args) -> {
            // Removing OLD operation variables, used to insert rules into P4 Controller. 
            // These rule describers, never exists anymore on the respective controller.
            createRules.getOperations().values().forEach(op -> {
                createRules.deleteSubmodelElement(op.getIdShort());
            });
            
            getRuleDescribers(controllerId, createRules);
            return new SubmodelElement[]{};
        };
    }
}