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

package p4_aas;

import org.eclipse.basyx.aas.metamodel.api.parts.asset.AssetKind;

import p4_aas.AssetShells.IShell;
import p4_aas.AssetShells.Machine;
import p4_aas.AssetShells.NetworkControlPlane;
import p4_aas.AssetShells.NetworkInfrastructure;
import p4_aas.NetworkController.NetworkController;

import java.util.List;

public class App {
    private final static String manual = "https://www.plattform-i40.de/IP/Redaktion/EN/Downloads/Publikation/Details_of_the_Asset_Administration_Shell_Part1_V3.pdf?__blob=publicationFile&v=1";
    private final static String version = "1.0.0";
    private final static NetworkController client = new NetworkController();
    public static void main(String[] args) {
        waitForRegistry();

        IShell networkInfrastructure = new NetworkInfrastructure(
            StaticProperties.NetworkInfrastructurePort, 
            "Network Infrastructure", 
            "org.unibo.aas.networkInfrastructure", 
            AssetKind.INSTANCE);

        IShell networkControlPlane = new NetworkControlPlane(
            StaticProperties.NetworkControlPlanePort, 
            "Network Control Plane", 
            "org.unibo.aas.networkControlPlane", 
            AssetKind.INSTANCE);

        IShell machineOne = new Machine(
            StaticProperties.MachineOnePort,
            "Machine1",
            "org.unibo.aas.machineOne",
            AssetKind.INSTANCE,
            manual,
            version,
            List.of(StaticProperties.Host1, StaticProperties.Host2));

        IShell machineTwo = new Machine(
            StaticProperties.MachineTwoPort,
            "Machine2",
            "org.unibo.aas.machineTwo",
            AssetKind.INSTANCE,
            manual,
            version,
            List.of(StaticProperties.Host3, StaticProperties.Host4));

        IShell machineThree = new Machine(
            StaticProperties.MachineThreePort,
            "Machine3",
            "org.unibo.aas.machineThree",
            AssetKind.INSTANCE,
            manual,
            version,
            List.of(StaticProperties.Host5, StaticProperties.Host6));

        networkInfrastructure.createAndStartServlet();
        networkControlPlane.createAndStartServlet();
        machineOne.createAndStartServlet();
        machineTwo.createAndStartServlet();
        machineThree.createAndStartServlet();
    }

    /**
     * Polling on Registry Url, waiting for a positive response to proceed on.
     */
    private static void waitForRegistry() {
        System.out.print("Waiting for Registry at " + StaticProperties.REGISTRY_POLLING_IP);
        while(!client.isServerAvailable(StaticProperties.REGISTRY_POLLING_IP)) {
            System.out.print(".");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {}
        }

        System.out.print("Waiting for CNT1 at " + StaticProperties.SW1_IP);
        while(!client.isServerAvailable(StaticProperties.SW1_IP)) {
            System.out.print(".");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {}
        }

        System.out.print("Waiting for CNT2 at " + StaticProperties.SW2_IP);
        while(!client.isServerAvailable(StaticProperties.SW2_IP)) {
            System.out.print(".");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {}
        }
    }
}