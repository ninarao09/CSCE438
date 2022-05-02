import java.io.IOException;
import java.util.StringTokenizer;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import java.io.IOException;
import java.util.Iterator;
import java.io.DataInput;
import java.io.DataOutput;
import java.lang.StringBuilder;
import java.lang.Iterable;
import java.util.List;
import java.util.Arrays;


import org.apache.hadoop.io.WritableComparable;

import org.apache.hadoop.io.LongWritable;

import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;


public class TimeOfDaySleep {

  public static class TokenizerMapper extends Mapper<Object, Text, Text, IntWritable>{

    private final static IntWritable one = new IntWritable(1);
    private Text time = new Text();


    public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
      StringTokenizer wordItr = new StringTokenizer(value.toString());
      List<String> linesInFile = Arrays.asList(value.toString().split("\n"));
	  Iterator<String> itr = linesInFile.iterator();

      Boolean sleepInTweet = false;
	  Boolean timeFound = false;
	
      while (itr.hasNext()) {
		
		String line = itr.next();


		// if(line.length() < 1){
		// 	continue;
		// }
			
			if(line.startsWith("T")){
				
				String [] theTime = line.split("\\s+");
					
						if(theTime[2].toString().startsWith("00:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("01:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("02:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("03:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("04:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("05:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("06:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("07:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("08:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("09:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("10:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("11:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("12:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("13:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("14:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("15:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("16:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("17:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("18:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("19:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("20:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("21:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("22:")){
							time.set(theTime[2].toString().substring(0,3));
						}else if(theTime[2].toString().startsWith("23:")){
							time.set(theTime[2].toString().substring(0,3));
						}
					
			}

			if(line.startsWith("W")){
				String line2 = line.toLowerCase();
				List<String> words = Arrays.asList(line2.split("\\s+"));
				// if(words.contains("sleep")){
				// 	context.write(time, one);
				// }
				for(String s : words){
					if(s.contains("sleep")){
						sleepInTweet = true;
					}
				}

				if(sleepInTweet){
					context.write(time, one);
					sleepInTweet = false;
				}
			}
		

      }
//hello
    }
  }

  public static class IntSumReducer
       extends Reducer<Text,IntWritable,Text,IntWritable> {
    private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values,
                       Context context
                       ) throws IOException, InterruptedException {
      int sum = 0;
      for (IntWritable val : values) {
        sum += val.get();
      }
      result.set(sum);
      context.write(key, result);
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    Job job = Job.getInstance(conf, "word count");
    job.setJarByClass(TimeOfDaySleep.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setCombinerClass(IntSumReducer.class);
    job.setReducerClass(IntSumReducer.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(IntWritable.class);
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}