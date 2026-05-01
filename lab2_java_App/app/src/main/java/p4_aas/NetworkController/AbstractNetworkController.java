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

package p4_aas.NetworkController;

import java.io.IOException;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

import org.apache.http.NameValuePair;
import org.apache.http.client.HttpResponseException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;

import p4_aas.NetworkController.Serialization.JsonObject;

public abstract class AbstractNetworkController {
    protected static final int HTTP_OK = 200;
    CloseableHttpClient apacheClient;
    ObjectMapper objMap;

    public AbstractNetworkController() {
        this.apacheClient = HttpClients.createDefault();
        this.objMap = new ObjectMapper();
    }

    /**
     * @param URL
     * @param nvps
     * @return Post request Status Code
     * @throws HttpResponseException 
     */
    public void postRequest(String URL, List<NameValuePair> nvps) {
        int statusCode = 0;
        String statusMessage = "";
        HttpPost httpPost = new HttpPost(URL);

        try {
            httpPost.setEntity(new UrlEncodedFormEntity(nvps));
            CloseableHttpResponse response = apacheClient.execute(httpPost);
            statusCode = response.getStatusLine().getStatusCode();
            statusMessage = response.getStatusLine().getReasonPhrase();
            response.close();
            httpPost.abort();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (statusCode != HTTP_OK)
            try {
                throw new HttpResponseException(statusCode, statusMessage);
            } catch (HttpResponseException e) {
                e.printStackTrace();
            }
    }

    public String getRequest(String URL) {
        int statusCode = 0;
        String statusMessage = "";
        String result = "";
        
        CloseableHttpResponse resp;
        try {
            resp = this.apacheClient.execute(new HttpGet(URL));
            System.out.println("URL: " + URL);
            System.out.println("RISULTATO: " + resp.getEntity());
            result = EntityUtils.toString(resp.getEntity());
            statusCode = resp.getStatusLine().getStatusCode();
            System.out.println("RISULTATO: " + resp.getEntity());
            statusMessage = resp.getStatusLine().getReasonPhrase();
            resp.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (statusCode != HTTP_OK)
            try {
                throw new HttpResponseException(statusCode, statusMessage);
            } catch (HttpResponseException e) {
                e.printStackTrace();
            }

        return result;
    }

    public List<String> jsonToList(String jsonString) {
        Gson gson = new Gson();
        JsonArray jsonArray = gson.fromJson(jsonString, JsonArray.class);

        return StreamSupport.stream(jsonArray.spliterator(), false)
                .map(JsonElement::toString)  // Converte ogni elemento del JsonArray in stringa
                .map(el -> {
                    String[] parts = el.split("->");
                    String index = parts[0].trim();  // Estrae l'indice
                    String jsonData = parts[1];     // Estrae il JSON vero e proprio

                    // Trasforma il JSON in formato richiesto
                    return index + " -> " + transformJson(jsonData);
                })
                .collect(Collectors.toList());
    }

    private static String transformJson(String jsonString) {
        Gson gson = new Gson();
        JsonObject json = gson.fromJson(jsonString, JsonObject.class);

        String table = json.Table.split("\\.")[1];
        String ip = json.Keys[0].Value;
        String action = json.Action.split("\\.")[1];
        String macAddress = json.ActionParam[0];
        String port = json.ActionParam[1];

        return String.format("[Table: %s]-[Ip: %s]-[Action: %s]-[DestinationPort: %s]-[DestinationMacAddress: %s]",
                table, ip, action, port, macAddress);
    }
}