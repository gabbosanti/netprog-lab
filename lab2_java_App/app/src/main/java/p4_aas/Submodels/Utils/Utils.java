// Copyright 2023 riccardo.bacca@studio.unibo.it
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package p4_aas.Submodels.Utils;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import org.eclipse.basyx.submodel.metamodel.api.qualifier.haskind.ModelingKind;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.Property;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.dataelement.property.valuetype.ValueType;
import org.eclipse.basyx.submodel.metamodel.map.submodelelement.operation.OperationVariable;

public class Utils {
    public static final int GET_FIREWALL_RULES = 15;

    /**
     * @param values Map of String and ValueType (String, Integer, Boolean)
     * @return list of OperationVariables for WebUI with ID and Type given from Map
     */
    public List<OperationVariable> getCustomInputVariables(Map<String, ValueType> values) {
        List<OperationVariable> rList = new LinkedList<>();

        values.forEach((k, v) -> {
            Property prop = new Property(k, v);
            prop.setKind(ModelingKind.TEMPLATE);
            rList.add(new OperationVariable(prop));
        });
        return rList;
    }

    /**
     * @param idShort
     * @param n how many Operation variables
     * @return N simple operation variables with ValueType String and standard IdShort
     */
    public List<OperationVariable> getOperationVariables(int n, String idShort) {
        return Collections.nCopies(n, new Property(idShort, ValueType.String)).
            stream().
            peek(el -> el.setKind(ModelingKind.TEMPLATE)).
            map(el -> new OperationVariable(el)).
            collect(Collectors.toList());
    }
}
