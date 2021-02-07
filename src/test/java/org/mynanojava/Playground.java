package org.mynanojava;
import org.junit.Test;
import org.mynanojava.blockchain.NanoBlock;
import org.mynanojava.wallet.NanoKeyPair;

import static org.mynanojava.blockchain.NanoBlock.THRESHOLD_RECEIVE;
import static org.mynanojava.enums.NanoAccountEnum.HEX_ACCOUNT;
import static org.mynanojava.enums.NanoAccountEnum.NANO_PREFIX;
import static org.mynanojava.enums.NanoJavaEnumBalanceType.NANO_BALANCE_REAL;
import static org.mynanojava.enums.NanoJavaEnumDirection.VALUE_TO_RECEIVE;
import static org.mynanojava.enums.NanoJavaEnumValueToSendOrReceive.NANO_VALUE_TO_SEND_OR_RECEIVE_REAL;

public class Playground {

    @Test
    public void testWallet() throws Throwable {
        //curl -d '{"action":"process","json_block":"true","block":{"type":"state","account":"nano_3o8dcsz5wrkumwwpn13jza6r78sbuiqih9chnfowpjs6qqa8apuuipo4ut99","previous":"D4CB567E3E625B9F396A0031FA09829B29DC2F079D4FA36BCB4724BDD0645B7B","representative":"nano_3ngt59dc7hbsjd1dum1bw9wbb87mbtuj4qkwcruididsb5rhgdt9zb4w7kb9","balance":"10000000000000000000000000000","link":"F2FB123830D10A91D2A189981E18EDFEEC6E3CD4D9BFEAFE7FEE4476E7143AB0","link_as_account":"nano_3wqu4aw53nack9bc54er5reguzqefryfbpfzxdz9zuk6gumjagoie4nqh5ci","signature":"4CE11B1D2DEF466C8B4E80C1AED31237510B3915F3B2E6708A77605CEC33BCA7F08CB72B2A17C158D3645FE35FB9EEC6E94F8909BD99BABD34235953B5BB9E0F","work":"B31DEE9520A85A2D"}}' -H "Content-Type: application/json" -X POST https://mynano.ninja/api/node
        final String SEED = "7B34126F69B0923736E71650A90EC3D6131B391649F27EA874BFA75C8ED04C31";

        long accountNumber = 0;
        NanoKeyPair keyPair = NanoBlock.fromNanoSeed(SEED, accountNumber);

        System.out.println("Wallet address number: " + accountNumber);
        System.out.println("Wallet address: " + keyPair.getAccount(NANO_PREFIX));
        System.out.println("Public key: " + keyPair.getAccount(HEX_ACCOUNT));
        System.out.println("Private key: " + keyPair.getPrivateKey());

        // Receiving amount
        NanoBlock nanoBlock = new MyNanoJava().nanoCreateBlock(
                keyPair.getAccount(NANO_PREFIX),
                null,
                "nano_3ngt59dc7hbsjd1dum1bw9wbb87mbtuj4qkwcruididsb5rhgdt9zb4w7kb9",
                "0.0",
                NANO_BALANCE_REAL.getValue(),
                "0.01",
                NANO_VALUE_TO_SEND_OR_RECEIVE_REAL.getValue(),
                "F2FB123830D10A91D2A189981E18EDFEEC6E3CD4D9BFEAFE7FEE4476E7143AB0",
                VALUE_TO_RECEIVE.getValue()
        );

        System.out.println("Signing block ...");
        nanoBlock.sign(keyPair.getPrivateKey());
        System.out.println("Calculating work ...");
        nanoBlock.calculateWork(THRESHOLD_RECEIVE, 4);
        System.out.println("JSON string with transaction: " + nanoBlock.toJson());

    }
}
