var fs = require("fs");
const http = require("http");
const speech = require("@google-cloud/speech");
const path = require("path");

const server = http.createServer();
const fileName = "./resources/recording.wav";
const tokenPath = path.join(__dirname, "google_service_tok.json"); // Adjust this path if needed

// Check if token file exists
if (!fs.existsSync(tokenPath)) {
    console.error(`Token file not found at ${tokenPath}`);
    process.exit(1);
}

// Creates a client with explicit credentials
const client = new speech.SpeechClient({
    keyFilename: tokenPath
});

server.on("request", (request, response) => {
    if (request.method == "POST" && request.url === "/uploadAudio") {
        var recordingFile = fs.createWriteStream(fileName, { encoding: "utf8" });
        request.on("data", function(data) {
            recordingFile.write(data);
        });

        request.on("end", async function() {
            recordingFile.end();
            try {
                const transcription = await speechToTextAPI();
                response.writeHead(200, { "Content-Type": "text/plain" });
                response.end(transcription);
            } catch (error) {
                console.error("Error in speech-to-text:", error);
                response.writeHead(500, { "Content-Type": "text/plain" });
                response.end("Error processing audio");
            }
        });
    } else {
        console.log("Error Check your POST request");
        response.writeHead(405, { "Content-Type": "text/plain" });
        response.end("Method Not Allowed");
    }
});

async function speechToTextAPI() {
    // Reads a local audio file and converts it to base64
    const file = fs.readFileSync(fileName);
    const audioBytes = file.toString("base64");

    // The audio file's encoding, sample rate in hertz, and BCP-47 language code
    const audio = {
        content: audioBytes
    };
    const config = {
        encoding: "LINEAR16",
        sampleRateHertz: 16000,
        languageCode: "en-US"
    };
    const request = {
        audio: audio,
        config: config
    };

    // Detects speech in the audio file
    const [response] = await client.recognize(request);
    const transcription = response.results.map((result) => result.alternatives[0].transcript).join("\n");
    console.log(`Transcription: ${transcription}`);
    return transcription;
}

const port = 8888;
const hostname = '0.0.0.0';  // This allows connections from any IP

server.listen(port,hostname, () => {
    console.log(`Server listening at hppt://${hostname}:${port}`);
    console.log(`Using token file: ${tokenPath}`);
});